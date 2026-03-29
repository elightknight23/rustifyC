; ModuleID = 'demo_testcases_for_teacher/02_spatial_safety_dynamic.c'
source_filename = "demo_testcases_for_teacher/02_spatial_safety_dynamic.c"
target datalayout = "e-m:o-i64:64-i128:128-n32:64-S128"
target triple = "arm64-apple-macosx16.0.0"

@.str = private unnamed_addr constant [48 x i8] c"Successfully wrote to buffer[%d]. Value is: %d\0A\00", align 1
@.rustifyc.panic.fmt = private constant [101 x i8] c"thread 'main' panicked at 'index out of bounds: the len is 10 but the index is %zu', unknown_file:0\0A\00"
@.rustifyc.panic.fmt.1 = private constant [101 x i8] c"thread 'main' panicked at 'index out of bounds: the len is 10 but the index is %zu', unknown_file:0\0A\00"

; Function Attrs: noinline nounwind optnone ssp uwtable
define i32 @main(i32 noundef %0, ptr noundef %1) #0 {
  %3 = alloca i32, align 4
  %4 = alloca i32, align 4
  %5 = alloca ptr, align 8
  %6 = alloca [10 x i32], align 4
  %7 = alloca i32, align 4
  store i32 0, ptr %3, align 4
  store i32 %0, ptr %4, align 4
  store ptr %1, ptr %5, align 8
  %8 = load ptr, ptr %5, align 8
  %9 = getelementptr inbounds ptr, ptr %8, i64 1
  %10 = load ptr, ptr %9, align 8
  %11 = call i32 @atoi(ptr noundef %10)
  store i32 %11, ptr %7, align 4
  %12 = load i32, ptr %7, align 4
  %13 = sext i32 %12 to i64
  %14 = icmp uge i64 %13, 10
  br i1 %14, label %25, label %15

15:                                               ; preds = %2
  %16 = getelementptr inbounds [10 x i32], ptr %6, i64 0, i64 %13
  store i32 42, ptr %16, align 4
  %17 = load i32, ptr %7, align 4
  %18 = load i32, ptr %7, align 4
  %19 = sext i32 %18 to i64
  %20 = icmp uge i64 %19, 10
  br i1 %20, label %27, label %21

21:                                               ; preds = %15
  %22 = getelementptr inbounds [10 x i32], ptr %6, i64 0, i64 %19
  %23 = load i32, ptr %22, align 4
  %24 = call i32 (ptr, ...) @printf(ptr noundef @.str, i32 noundef %17, i32 noundef %23)
  ret i32 0

25:                                               ; preds = %2
  %26 = call i32 (ptr, ...) @printf(ptr @.rustifyc.panic.fmt, i64 %13)
  call void @exit(i32 101)
  unreachable

27:                                               ; preds = %15
  %28 = call i32 (ptr, ...) @printf(ptr @.rustifyc.panic.fmt.1, i64 %19)
  call void @exit(i32 101)
  unreachable
}

declare i32 @atoi(ptr noundef) #1

declare i32 @printf(ptr noundef, ...) #1

declare i32 @fprintf(ptr, ptr, ...)

declare void @exit(i32)

attributes #0 = { noinline nounwind optnone ssp uwtable "frame-pointer"="non-leaf" "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="apple-m1" "target-features"="+aes,+crc,+crypto,+dotprod,+fp-armv8,+fp16fml,+fullfp16,+lse,+neon,+ras,+rcpc,+rdm,+sha2,+sha3,+sm4,+v8.5a,+zcm,+zcz" }
attributes #1 = { "frame-pointer"="non-leaf" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="apple-m1" "target-features"="+aes,+crc,+crypto,+dotprod,+fp-armv8,+fp16fml,+fullfp16,+lse,+neon,+ras,+rcpc,+rdm,+sha2,+sha3,+sm4,+v8.5a,+zcm,+zcz" }

!llvm.module.flags = !{!0, !1, !2, !3}
!llvm.ident = !{!4}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{i32 7, !"PIC Level", i32 2}
!2 = !{i32 7, !"uwtable", i32 2}
!3 = !{i32 7, !"frame-pointer", i32 1}
!4 = !{!"Homebrew clang version 15.0.7"}
