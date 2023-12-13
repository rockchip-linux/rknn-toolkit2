
#ifndef API_EXPORT_H
#define API_EXPORT_H

#ifdef MediaKitApi_STATIC
#  define API_EXPORT
#  define MK_API_NO_EXPORT
#else
#  ifndef API_EXPORT
#    ifdef mk_api_EXPORTS
        /* We are building this library */
#      define API_EXPORT __attribute__((visibility("default")))
#    else
        /* We are using this library */
#      define API_EXPORT __attribute__((visibility("default")))
#    endif
#  endif

#  ifndef MK_API_NO_EXPORT
#    define MK_API_NO_EXPORT __attribute__((visibility("hidden")))
#  endif
#endif

#ifndef MK_API_DEPRECATED
#  define MK_API_DEPRECATED __attribute__ ((__deprecated__))
#endif

#ifndef MK_API_DEPRECATED_EXPORT
#  define MK_API_DEPRECATED_EXPORT API_EXPORT MK_API_DEPRECATED
#endif

#ifndef MK_API_DEPRECATED_NO_EXPORT
#  define MK_API_DEPRECATED_NO_EXPORT MK_API_NO_EXPORT MK_API_DEPRECATED
#endif

#if 0 /* DEFINE_NO_DEPRECATED */
#  ifndef MK_API_NO_DEPRECATED
#    define MK_API_NO_DEPRECATED
#  endif
#endif

#endif /* API_EXPORT_H */
