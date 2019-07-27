load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

http_archive(
    name = "murtis_bazel_compilers",
    urls = [
      "https://github.com/curtismuntz/bazel_compilers/archive/a2413f93c9b866b20d4f2923b4ba329fd684f93d.tar.gz",
    ],
    strip_prefix = "bazel_compilers-a2413f93c9b866b20d4f2923b4ba329fd684f93d",
)

load("@murtis_bazel_compilers//compilers:dependencies.bzl", "cross_compiler_dependencies")

cross_compiler_dependencies()
