load("@rules_cc//cc:defs.bzl", "cc_binary")

cc_binary(
    name = "day-1",
    srcs = ["day_1_report_repair.cc"],
    # copts = ["-std=c++20"], # This doesn't work because it won't build absl with C++20 which results in linker errors
    deps = [
        "@com_google_absl//absl/status:status",
        "@com_google_absl//absl/status:statusor",
        "@com_google_absl//absl/strings:str_format",
    ],
)


cc_binary(
    name = "day-2",
    srcs = ["day_2_password_philosophy.cc"],
    deps = [
        "@com_google_absl//absl/strings",
        "@com_google_absl//absl/strings:str_format",
    ],
)
