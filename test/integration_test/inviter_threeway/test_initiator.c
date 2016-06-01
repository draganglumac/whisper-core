/*
 * =====================================================================================
 *
 *       Filename:  test_session.c
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  02/09/2015 05:57:10 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (),
 *   Organization:
 *
 * =====================================================================================
 */
#include <stdlib.h>
#include <jnxc_headers/jnxsocket.h>
#include <jnxc_headers/jnxthread.h>
#include <jnxc_headers/jnxlog.h>
#include <jnxc_headers/jnx_tcp_socket.h>
#include "session_service.h"
#include "port_control.h"
#include "auth_comms.h"
#include "discovery.h"
static char *baddr = NULL;
static char *interface = NULL;
static auth_comms_service *ac = NULL;
static port_control_service *ps = NULL;
int app_accept_reject(discovery_service *ds, jnx_guid *initiator_guild,
    jnx_guid *session_guid) {
  return 0;
}
int accept_invite_callback(jnx_guid *session_guid) {

  return 1;
}
int linking_test_procedure(session *s,linked_session_type session_type,
    void *optargs) {
  if(session_type == E_AM_INITIATOR){
    JNXCHECK(session_type == E_AM_INITIATOR);
    JNXLOG(NULL,"Session hit linking procedure functor");
    discovery_service *ds = (discovery_service*)optargs;
    /* Adding port control service */


    auth_comms_initiator_start(ac,ds,ps,s,"Hello from the initiator!");
  }
  return 0;
}
int unlinking_test_procedure(session *s,linked_session_type session_type,
    void *optargs) {

  auth_comms_stop(ac,s);
  return 0;
}
void test_initiator() {
  JNXLOG(NULL,"test_linking");
 
  int init_port = rand() % 1000;
 ps = port_control_service_create(8000 + 
        init_port,
        12341,1);
  
  ac = auth_comms_create();
  ac->listener = jnx_socket_tcp_listener_create("9991",AF_INET,15);
  ac->ar_callback = app_accept_reject;
  ac->invitation_callback = accept_invite_callback;

  session_service *service = session_service_create(linking_test_procedure,
      unlinking_test_procedure);
  session *os;
  session_state e = session_service_create_session(service,&os);
  JNXCHECK(session_service_session_is_linked(service,&os->session_guid) == 0);
  //Lets generate the guid of some remote session
  jnx_guid h;
  jnx_guid_create(&h);

  peerstore *store = peerstore_init(local_peer_for_user("initiator_bob",10,interface), 0);

  get_broadcast_ip(&baddr,interface);
  printf("%s\n", baddr);
  discovery_service *ds = discovery_service_create(1234, AF_INET, baddr, store);

  discovery_service_start(ds,BROADCAST_UPDATE_STRATEGY);

  auth_comms_listener_start(ac,ds,service,ps,NULL);

  int remote_peers = 0;
  int has_remote_invitee = 0;
  jnx_guid **active_guids;
  peer *local = peerstore_get_local_peer(store);
  peer *remote_peer = NULL;
  peer *remote_invitee = NULL;

  while(!remote_peers) {
    int num_guids = peerstore_get_active_guids(store,&active_guids);
    int i;
    for(i=0;i<num_guids;i++) {
      jnx_guid *guid = active_guids[i];
      peer *p = peerstore_lookup(store,guid);
      if(peers_compare(p,local) != 0) {
        printf("Found a remote peer! Breaking!\n");
        remote_peers = 1;
        remote_peer = p;
        break;
      }
    }
  }
  printf("Found remote peer and going to create session with => ");
  print_peer(remote_peer);
  printf("\n");

  JNXCHECK(session_is_active(os) == 0);

  session_service_link_sessions(service,E_AM_INITIATOR,
      ds,&(*os).session_guid,local,remote_peer);

  JNXCHECK(session_is_active(os) == 1);

  printf("-------------------------------------\n");
  session_message_write(os,"Hello Ballface! what's going on!");
  printf("-------------------------------------\n");

  printf("About to invite peer C\n");

  /* lets invite Peer C in */
  while(!remote_invitee) {
    int num_guids = peerstore_get_active_guids(store,&active_guids);
    int i;
    for(i=0;i<num_guids;i++) {
      jnx_guid *guid = active_guids[i];
      peer *p = peerstore_lookup(store,guid);
      if(peers_compare(p,local) != 0 && peers_compare(p,remote_peer) != 0) {
        printf("Found a remote invitee! Breaking!\n");
        has_remote_invitee = 1;
        remote_invitee = p;
        break;
      }
    }
  }
  
  sleep(1);

  printf("Found remote invitee and going to send invite => ");
  print_peer(remote_invitee);
  printf("\n");
  auth_comms_invite_send(ac,os,remote_invitee);

  int is_invitee_session_active = 0;

  session *remote_invitee_session = NULL;

  while(!is_invitee_session_active) {
    
    sleep(1);
    
    jnx_list *olist = NULL;
    session_service_fetch_all_sessions(service,&olist);

    if(olist->counter < 2) {
      jnx_list_destroy(&olist);
      JNXLOG(LDEBUG,"Awaiting second session...");
      continue;
    }
    JNXLOG(LDEBUG,"Found second session...");
    
    jnx_node *n = olist->head;

    while(n) {
    
      session *s = n->_data;

      if(jnx_guid_compare(&(*s).session_guid,&(*os).session_guid )
          == JNX_GUID_STATE_FAILURE) {
        remote_invitee_session = s; 
        is_invitee_session_active = 1; 
        JNXLOG(LDEBUG,"Found remote session breaking!!!");
        break;
      }
      JNXLOG(LDEBUG,"Awaiting valid session...");
      n = n->next_node;
    }
  }

  JNXLOG(LDEBUG,"Remote invitee session is active!");
  sleep(10);
  
  session_message_write(remote_invitee_session,"213 Regulate!");
  sleep(2);
}
int main(int argc, char **argv) {
  if (argc > 1) {
    interface = argv[1];
    printf("using interface %s", interface);
  }
  JNXLOG_CREATE("logger.conf");
  test_initiator();
  return 0;
}