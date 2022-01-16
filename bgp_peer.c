#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <stdint.h>
#include <stdbool.h>
#include <assert.h>
#include "gluethread/glthread.h"
#include "CommandParser/libcli.h"
#include "CommandParser/cmdtlv.h"
#include "libtimer/WheelTimer.h"
#include "bgppeer.h"
#include "bgpcmdcodes.h"

/* Sources :
https://yurmagccie.wordpress.com/2018/05/19/bgp-part-1-finite-state-machine/
https://nwktimes.blogspot.com/2017/07/border-gateway-protocol-finite-state.html
*/
static void
bgp_transition_tcp_conn_state_to_new_state (
        bgp_tcp_conn_t *bgp_tcp_conn,
        bgp_peer_conn_state_t new_state) {

    if (bgp_tcp_conn->conn_state == new_state) return;

    switch (new_state) {
         case BGP_IDLE:
                /* Algorithm
            0. Drop the TCP connection if established, refuse all incoming connections
            1. Initialize all BGP Resources for the peer connection
            2. sets ConnectRetryCounter to zero,
                bgp_config_peer_t->bgp_peer_info->peer_conn->connect_conn->connect_retry_count = 0
            3. starts the ConnectRetryTimer with the initial value
                bgp_config_peer_t->bgp_peer_info->peer_conn->connect_retry_timer = <initialize>
            4. initiates a TCP connection to the other BGP peer
                call to connect( ) to peer. Note connect() is a blocking call.
            5. listens for a connection that may be initiated by the remote BGP peer
            6. Move to BGP_CONNECT irrespective (4) succeeds or not
            */
                break;
            case BGP_CONNECT:
              /* Algorithm 
            BGP is waiting for the TCP connection to be completed, either passively or actively
            1. if connect() succeeds, move to BGP_OPEN_SENT state, stop ConnectRetryTimer, send OPEN MSG
            2. if accept() succeeds, move to BGP_OPEN_SENT state, stop ConnectRetryTimer, send OPEN MSG
            3. if both - connect() & accept( ) succeeds, trigger tcp connection collision resolution
            4. if ConnectRetryTimer( ) expires, restart the ConnectRetryTimer( ) , call connect() again, stays in BGP_CONNECT( )
            5. If connect() fails, move to BGP_ACTIVE state, restart ConnectRetryTimer( )
            */
                break;
            case BGP_ACTIVE:
                /* Algorithm
                BGP is looking to initiate TCP connection with remote peer
                1. if ConnectRetryTimer( ) expires, move back to BGP_CONNECT state
                2. if accept() succeeds, move to BGP_OPEN_SENT state, stop ConnectRetryTimer, send OPEN MSG
                3. if Connection is UP, and OPen msg is recvd, then move to BGP_OPEN_CONFIRM state
                */
                break;
            case BGP_OPEN_SENT:
                break;
            case BGP_OPEN_CONFIRM:
                break;
            case BGP_ESTABLISHED:
                break;
            default:;
    }
}


static bool
bgp_move_peer_tcp_conn_fromstate_to_tostate (
        bgp_peer_conn_t *bgp_peer_conn,
        bgp_peer_conn_state_t new_state) {

    bgp_peer_conn_state_t old_state;
    bool via_connect = false;
    bool via_accept = false;

    bgp_tcp_conn_t *tcp_conn;

   if (bgp_peer_conn->accept_conn) {
       tcp_conn = bgp_peer_conn->accept_conn;
       via_accept = true;
   }
   else if (bgp_peer_conn->connect_conn) {
       tcp_conn = bgp_peer_conn->connect_conn;
       via_connect = true;
   }
   else return false;
   
    old_state = tcp_conn->conn_state;

    switch(old_state) {

        case BGP_IDLE:

            switch (new_state)
            {
            case BGP_IDLE:
                break;
            case BGP_CONNECT:
                break;
            case BGP_ACTIVE:
                break;
            case BGP_OPEN_SENT:
                break;
            case BGP_OPEN_CONFIRM:
                break;
            case BGP_ESTABLISHED:
                break;
            default:;
            }

        break;


        case BGP_CONNECT:

         switch (new_state)
            {
            case BGP_IDLE:
                break;
            case BGP_CONNECT:
                break;
            case BGP_ACTIVE:
            /* Algorithm */
                break;
            case BGP_OPEN_SENT:
                break;
            case BGP_OPEN_CONFIRM:
                break;
            case BGP_ESTABLISHED:
                break;
            default:;
            }

        break;


        case BGP_ACTIVE:

         switch (new_state)
            {
            case BGP_IDLE:
                break;
            case BGP_CONNECT:
                break;
            case BGP_ACTIVE:
                break;
            case BGP_OPEN_SENT:
                break;
            case BGP_OPEN_CONFIRM:
                break;
            case BGP_ESTABLISHED:
                break;
            default:;
            }

        break;


        case BGP_OPEN_SENT:

         switch (new_state)
            {

            case BGP_IDLE:
                break;
            case BGP_CONNECT:
                break;
            case BGP_ACTIVE:
                break;
            case BGP_OPEN_SENT:
                break;
            case BGP_OPEN_CONFIRM:
                break;
            case BGP_ESTABLISHED:
                break;
            default:;
            }

        break;


        case BGP_OPEN_CONFIRM:

         switch (new_state)
            {

            case BGP_IDLE:
                break;
            case BGP_CONNECT:
                break;
            case BGP_ACTIVE:
                break;
            case BGP_OPEN_SENT:
                break;
            case BGP_OPEN_CONFIRM:
                break;
            case BGP_ESTABLISHED:
                break;
            default:;
            }

        break;


        case BGP_ESTABLISHED:

         switch (new_state)
            {

            case BGP_IDLE:
                break;
            case BGP_CONNECT:
                break;
            case BGP_ACTIVE:
                break;
            case BGP_OPEN_SENT:
                break;
            case BGP_OPEN_CONFIRM:
                break;
            case BGP_ESTABLISHED:
                break;
            default:;
            }

        break;

        default : ;
    }

}

void
bgp_apply_event_on_connection (
        bgp_peer_conn_t *bgp_peer_conn,
        bgp_event_type_t event) {

    bgp_peer_conn_state_t old_state;
    bool via_connect = false;
    bool via_accept = false;

    bgp_tcp_conn_t *tcp_conn;

   if (bgp_peer_conn->accept_conn) {
       tcp_conn = bgp_peer_conn->accept_conn;
       via_accept = true;
   }
   else if (bgp_peer_conn->connect_conn) {
       tcp_conn = bgp_peer_conn->connect_conn;
       via_connect = true;
   }
   else return;

   switch (tcp_conn->conn_state)
   {

   case BGP_IDLE:
       switch (event)
       {

       case BGP_MANUAL_STOP_EVENT:
           /* no action */
           break;
       case BGP_AUTOMATIC_STOP_EVENT:
           /* no action */
           break;
       default:;
       }
       break;

   case BGP_CONNECT:

       switch (event)
       {

       case BGP_MANUAL_STOP_EVENT:
       case BGP_AUTOMATIC_STOP_EVENT:
           bgp_move_peer_tcp_conn_fromstate_to_tostate(bgp_peer_conn, BGP_IDLE);
           break;
       default:;
       }

       break;
   case BGP_ACTIVE:
       break;
   case BGP_OPEN_SENT:
       break;
   case BGP_OPEN_CONFIRM:
       break;
   case BGP_ESTABLISHED:
       break;
   default:;
   }
}
