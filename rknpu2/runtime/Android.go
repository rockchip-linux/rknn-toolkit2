package librknnrt

import (
    "android/soong/android"
    "android/soong/cc"
    "fmt"
    "strings"
)

var SUPPORT_TARGET_PLATFORM = [...]string{
    "rk356x",
    "rk3588",
}

func init() {
    fmt.Println("librknnrt want to conditional Compile")
    android.RegisterModuleType("cc_librknnrt_prebuilt_library_shared", LibrknnrtFactory)
    // android.RegisterModuleType("librknnrt_headers", LibrknnrtHeadersFactory)
}

func LibrknnrtFactory() (android.Module) {
    module := cc.PrebuiltSharedLibraryFactory()
    android.AddLoadHook(module, LibrknnrtPrebuiltLibrary)
    return module
}

func LibrknnrtPrebuiltLibrary(ctx android.LoadHookContext) {

    type props struct {
        Multilib struct {
            Lib64 struct {
                Srcs []string
            }
            Lib32 struct {
                Srcs []string
            }
        }
        Export_include_dirs []string
    }
    p := &props{}

    p.Multilib.Lib64.Srcs = getLibrknnrtLibrary(ctx, "arm64-v8a")
    p.Multilib.Lib32.Srcs = getLibrknnrtLibrary(ctx, "armeabi-v7a")
    p.Export_include_dirs = getLibrknnrtHeader(ctx)
    ctx.AppendProperties(p)
}

// func LibrknnrtHeadersFactory() (android.Module) {
//     module := cc.prebuiltLibraryHeaderFactory()
//     android.AddLoadHook(module, LibrknnrtHeaders)
//     return module
// }

// func LibrknnrtHeaders(ctx android.LoadHookContext) {

//     type props struct {
//         Export_include_dirs []string
//     }
//     p := &props{}

//     p.Export_include_dirs = getLibrknnrtHeader(ctx)
//     ctx.AppendProperties(p)
// }

func checkEnabled(ctx android.LoadHookContext) bool {
    var soc string = getTargetSoc(ctx)
    for i := 0; i < len(SUPPORT_TARGET_PLATFORM); i++ {
        if (strings.EqualFold(SUPPORT_TARGET_PLATFORM[i], soc)) {
            fmt.Println("librknnrt enabled on " + soc)
            return true
        }
    }
    fmt.Println("librknnrt disabled on " + soc)
    return false
}

func getLibrknnrtLibrary(ctx android.LoadHookContext, arch string) ([]string) {
    var src []string

    src = append(src, "/Android/librknn_api/" + arch + "/librknnrt.so")

    return src
}

func getLibrknnrtHeader(ctx android.LoadHookContext) ([]string) {
    var src []string

    src = append(src, "/Android/librknn_api/include/")

    return src
}

func getTargetSoc(ctx android.LoadHookContext) (string) {
    var target_board_platform string = strings.ToUpper(ctx.AConfig().Getenv("TARGET_BOARD_PLATFORM"))
    return target_board_platform
}
