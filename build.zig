const std = @import("std");

pub fn build(b: *std.Build) void {
    const target = b.standardTargetOptions(.{});
    const optimize = b.standardOptimizeOption(.{});

    // Common C++ flags
    const cpp_flags: []const []const u8 = &.{
        "-std=c++20",
        "-O2",
    };

    // =========================================
    // Static Library: LimitOrderBook_lib
    // =========================================
    const lib_module = b.createModule(.{
        .target = target,
        .optimize = optimize,
        .link_libcpp = true,
    });

    lib_module.addCSourceFiles(.{
        .files = &.{
            "Limit_Order_Book/Book.cpp",
            "Limit_Order_Book/Limit.cpp",
            "Limit_Order_Book/Order.cpp",
            "Process_Orders/OrderPipeline.cpp",
            "Generate_Orders/GenerateOrders.cpp",
        },
        .flags = cpp_flags,
    });

    const lib = b.addLibrary(.{
        .name = "LimitOrderBook_lib",
        .root_module = lib_module,
    });
    b.installArtifact(lib);

    // =========================================
    // Main Executable: LimitOrderBook
    // =========================================
    const exe_module = b.createModule(.{
        .target = target,
        .optimize = optimize,
        .link_libcpp = true,
    });

    exe_module.addCSourceFiles(.{
        .files = &.{"main.cpp"},
        .flags = cpp_flags,
    });

    exe_module.linkLibrary(lib);

    const exe = b.addExecutable(.{
        .name = "LimitOrderBook",
        .root_module = exe_module,
    });
    b.installArtifact(exe);

    // Run command: zig build run
    const run_cmd = b.addRunArtifact(exe);
    run_cmd.step.dependOn(b.getInstallStep());
    if (b.args) |args| {
        run_cmd.addArgs(args);
    }

    const run_step = b.step("run", "Run the limit order book");
    run_step.dependOn(&run_cmd.step);

    // =========================================
    // GoogleTest Library
    // =========================================
    const gtest_module = b.createModule(.{
        .target = target,
        .optimize = optimize,
        .link_libcpp = true,
    });

    gtest_module.addCSourceFiles(.{
        .files = &.{
            "googletest/googletest/src/gtest-all.cc",
            "googletest/googletest/src/gtest_main.cc",
        },
        .flags = cpp_flags,
    });

    gtest_module.addIncludePath(b.path("googletest/googletest/include"));
    gtest_module.addIncludePath(b.path("googletest/googletest"));

    const gtest = b.addLibrary(.{
        .name = "gtest",
        .root_module = gtest_module,
    });

    // =========================================
    // Test Executable: LimitOrderBookTests
    // =========================================
    const test_module = b.createModule(.{
        .target = target,
        .optimize = optimize,
        .link_libcpp = true,
    });

    test_module.addCSourceFiles(.{
        .files = &.{
            "test/LimitOrderBookTests.cpp",
            "test/ExampleOrdersTests.cpp",
        },
        .flags = cpp_flags,
    });

    test_module.addIncludePath(b.path("googletest/googletest/include"));
    test_module.linkLibrary(lib);
    test_module.linkLibrary(gtest);

    const tests = b.addExecutable(.{
        .name = "LimitOrderBookTests",
        .root_module = test_module,
    });
    b.installArtifact(tests);

    // Run tests: zig build test
    const run_tests = b.addRunArtifact(tests);
    run_tests.step.dependOn(b.getInstallStep());

    const test_step = b.step("test", "Run unit tests");
    test_step.dependOn(&run_tests.step);

    // Clean step: zig build clean
    const clean_step = b.step("clean", "Remove build artifacts");
    clean_step.dependOn(&b.addRemoveDirTree(b.path(".zig-cache")).step);
    clean_step.dependOn(&b.addRemoveDirTree(b.path("zig-out")).step);
}
