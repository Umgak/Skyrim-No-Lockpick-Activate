#ifndef NLA_VERSION_INCLUDED
#define NLA_VERSION_INCLUDED

#define MAKE_STR_HELPER(a_str) #a_str
#define MAKE_STR(a_str) MAKE_STR_HELPER(a_str)

#define NLA_VERSION_MAJOR	3
#define NLA_VERSION_MINOR	1
#define NLA_VERSION_PATCH	0
#define NLA_VERSION_BETA	0
#define NLA_VERSION_VERSTRING	MAKE_STR(NLA_VERSION_MAJOR) "." MAKE_STR(NLA_VERSION_MINOR) "." MAKE_STR(NLA_VERSION_PATCH) "." MAKE_STR(NLA_VERSION_BETA)

#endif