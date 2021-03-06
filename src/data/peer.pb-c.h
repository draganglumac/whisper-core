/* Generated by the protocol buffer compiler.  DO NOT EDIT! */
/* Generated from: peer.proto */

#ifndef PROTOBUF_C_peer_2eproto__INCLUDED
#define PROTOBUF_C_peer_2eproto__INCLUDED

#include <protobuf-c/protobuf-c.h>

PROTOBUF_C__BEGIN_DECLS

#if PROTOBUF_C_VERSION_NUMBER < 1000000
# error This file was generated by a newer version of protoc-c which is incompatible with your libprotobuf-c headers. Please update your headers.
#elif 1002001 < PROTOBUF_C_MIN_COMPILER_VERSION
# error This file was generated by an older version of protoc-c which is incompatible with your libprotobuf-c headers. Please regenerate this file with a newer version of protoc-c.
#endif


typedef struct _Peer Peer;


/* --- enums --- */


/* --- messages --- */

struct  _Peer
{
  ProtobufCMessage base;
  ProtobufCBinaryData guid;
  char *host_address;
  char *user_name;
  uint32_t discovery_interval;
};
#define PEER__INIT \
 { PROTOBUF_C_MESSAGE_INIT (&peer__descriptor) \
    , {0,NULL}, NULL, NULL, 0 }


/* Peer methods */
void   peer__init
                     (Peer         *message);
size_t peer__get_packed_size
                     (const Peer   *message);
size_t peer__pack
                     (const Peer   *message,
                      uint8_t             *out);
size_t peer__pack_to_buffer
                     (const Peer   *message,
                      ProtobufCBuffer     *buffer);
Peer *
       peer__unpack
                     (ProtobufCAllocator  *allocator,
                      size_t               len,
                      const uint8_t       *data);
void   peer__free_unpacked
                     (Peer *message,
                      ProtobufCAllocator *allocator);
/* --- per-message closures --- */

typedef void (*Peer_Closure)
                 (const Peer *message,
                  void *closure_data);

/* --- services --- */


/* --- descriptors --- */

extern const ProtobufCMessageDescriptor peer__descriptor;

PROTOBUF_C__END_DECLS


#endif  /* PROTOBUF_C_peer_2eproto__INCLUDED */
