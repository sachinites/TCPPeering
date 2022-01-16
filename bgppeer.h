#ifndef __BGP_PEER__
#define __BGP_PEER__

#define BGP_DEFAULT_LISTENING_PORT  179

typedef enum {

    tcp_conn_type_incoming,  /* created using accept() */
    tcp_conn_type_outgoing   /* created using connect() */
} tcp_conn_type_t;

typedef enum {

    BGP_IDLE,
    BGP_CONNECT,
    BGP_ACTIVE,
    BGP_OPEN_SENT,
    BGP_OPEN_CONFIRM,
    BGP_ESTABLISHED,
    BGP_CONN_MAX
} bgp_peer_conn_state_t;

typedef struct bgp_tcp_conn_ {

    uint32_t peer_addr;
    uint16_t peer_tcp_port_no;
    uint16_t self_tcp_port_no;
    tcp_conn_type_t tcp_conn_type;
    bgp_peer_conn_state_t conn_state;
    uint16_t sockfd; /* returned by connect() or accept() */

    /* Applicable only when this connection is created via connect() */
    wheel_timer_elem_t *connect_retry_timer;
    uint16_t connect_retry_count;

     wheel_timer_elem_t *ka_timer;

    /* Stats */
    uint32_t total_msg_sent;
    uint32_t total_msg_recvd;
} bgp_tcp_conn_t;

typedef struct bgp_peer_conn_ {

    /* Connection established when this node sends connect to peer */
    bgp_tcp_conn_t *connect_conn;
    /* Connection established when this node accepts() from peer */
    bgp_tcp_conn_t *accept_conn;
} bgp_peer_conn_t;

typedef struct bgp_peer_info_ {

    bgp_peer_conn_t *peer_conn;
    /* Stats */
    uint32_t total_msg_sent;
    uint32_t total_msg_recvd;
} bgp_peer_info_t;

typedef struct bgp_config_peer_ {

    uint32_t peer_addr;
    uint16_t hold_time;
    bgp_peer_info_t *bgp_peer_info;
    glthread_t config_peer_list_head_glue;
} bgp_config_peer_t;

typedef struct bgp_node_ {

    uint32_t rtr_id;
    uint16_t listening_tcp_port_no;
    uint16_t master_tcp_sock_fd;
    glthread_t config_peer_list_head;
} bgp_node_t;

/* Event */
typedef enum {

    BGP_MANUAL_START_EVENT = 1,         // nbr configured
    BGP_MANUAL_STOP_EVENT = 2,           // nbr unconfigured
    BGP_AUTOMATIC_START_EVENT = 7,   // nbr configured
    BGP_AUTOMATIC_STOP_EVENT = 8,     // nbr unconfigured
    BGP_EVENT_MAX
} bgp_event_type_t;

bool bgp_add_new_peer (char *peer_ip_addr);
bool bgp_remove_peer(char *peer_ip_addr);
bool bgp_terminate_peer_tcp_connection(char *peer_ip_addr);

void
bgp_apply_event_on_connection (bgp_peer_conn_t *bgp_peer_conn, bgp_event_type_t event);
#endif 