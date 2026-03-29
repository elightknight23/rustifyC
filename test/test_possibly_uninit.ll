; ModuleID = 'test_possibly_uninit.c'
source_filename = "test_possibly_uninit.c"
target datalayout = "e-m:o-i64:64-i128:128-n32:64-S128"
target triple = "arm64-apple-macosx16.0.0"

@.str = private unnamed_addr constant [12 x i8] c"Result: %d\0A\00", align 1, !dbg !0

; Function Attrs: noinline nounwind ssp uwtable
define void @conditional_init(i32 noundef %0) #0 !dbg !16 {
  %2 = alloca i32, align 4
  %3 = alloca i32, align 4
  store i32 %0, ptr %2, align 4
  call void @llvm.dbg.declare(metadata ptr %2, metadata !21, metadata !DIExpression()), !dbg !22
  call void @llvm.dbg.declare(metadata ptr %3, metadata !23, metadata !DIExpression()), !dbg !24
  %4 = load i32, ptr %2, align 4, !dbg !25
  %5 = icmp ne i32 %4, 0, !dbg !25
  br i1 %5, label %6, label %7, !dbg !27

6:                                                ; preds = %1
  store i32 5, ptr %3, align 4, !dbg !28
  br label %7, !dbg !30

7:                                                ; preds = %6, %1
  %8 = load i32, ptr %3, align 4, !dbg !31
  %9 = call i32 (ptr, ...) @printf(ptr noundef @.str, i32 noundef %8), !dbg !32
  ret void, !dbg !33
}

; Function Attrs: nocallback nofree nosync nounwind readnone speculatable willreturn
declare void @llvm.dbg.declare(metadata, metadata, metadata) #1

declare i32 @printf(ptr noundef, ...) #2

; Function Attrs: noinline nounwind ssp uwtable
define i32 @main() #0 !dbg !34 {
  %1 = alloca i32, align 4
  store i32 0, ptr %1, align 4
  call void @conditional_init(i32 noundef 1), !dbg !37
  ret i32 0, !dbg !38
}

attributes #0 = { noinline nounwind ssp uwtable "frame-pointer"="non-leaf" "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="apple-m1" "target-features"="+aes,+crc,+crypto,+dotprod,+fp-armv8,+fp16fml,+fullfp16,+lse,+neon,+ras,+rcpc,+rdm,+sha2,+sha3,+sm4,+v8.5a,+zcm,+zcz" }
attributes #1 = { nocallback nofree nosync nounwind readnone speculatable willreturn }
attributes #2 = { "frame-pointer"="non-leaf" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="apple-m1" "target-features"="+aes,+crc,+crypto,+dotprod,+fp-armv8,+fp16fml,+fullfp16,+lse,+neon,+ras,+rcpc,+rdm,+sha2,+sha3,+sm4,+v8.5a,+zcm,+zcz" }

!llvm.dbg.cu = !{!7}
!llvm.module.flags = !{!9, !10, !11, !12, !13, !14}
!llvm.ident = !{!15}

!0 = !DIGlobalVariableExpression(var: !1, expr: !DIExpression())
!1 = distinct !DIGlobalVariable(scope: null, file: !2, line: 9, type: !3, isLocal: true, isDefinition: true)
!2 = !DIFile(filename: "test_possibly_uninit.c", directory: "/Users/nithikdeva/rustifyC/test")
!3 = !DICompositeType(tag: DW_TAG_array_type, baseType: !4, size: 96, elements: !5)
!4 = !DIBasicType(name: "char", size: 8, encoding: DW_ATE_signed_char)
!5 = !{!6}
!6 = !DISubrange(count: 12)
!7 = distinct !DICompileUnit(language: DW_LANG_C99, file: !2, producer: "Homebrew clang version 15.0.7", isOptimized: false, runtimeVersion: 0, emissionKind: FullDebug, globals: !8, splitDebugInlining: false, nameTableKind: None, sysroot: "/Library/Developer/CommandLineTools/SDKs/MacOSX14.sdk", sdk: "MacOSX14.sdk")
!8 = !{!0}
!9 = !{i32 7, !"Dwarf Version", i32 4}
!10 = !{i32 2, !"Debug Info Version", i32 3}
!11 = !{i32 1, !"wchar_size", i32 4}
!12 = !{i32 7, !"PIC Level", i32 2}
!13 = !{i32 7, !"uwtable", i32 2}
!14 = !{i32 7, !"frame-pointer", i32 1}
!15 = !{!"Homebrew clang version 15.0.7"}
!16 = distinct !DISubprogram(name: "conditional_init", scope: !2, file: !2, line: 3, type: !17, scopeLine: 3, flags: DIFlagPrototyped, spFlags: DISPFlagDefinition, unit: !7, retainedNodes: !20)
!17 = !DISubroutineType(types: !18)
!18 = !{null, !19}
!19 = !DIBasicType(name: "int", size: 32, encoding: DW_ATE_signed)
!20 = !{}
!21 = !DILocalVariable(name: "flag", arg: 1, scope: !16, file: !2, line: 3, type: !19)
!22 = !DILocation(line: 3, column: 27, scope: !16)
!23 = !DILocalVariable(name: "x", scope: !16, file: !2, line: 4, type: !19)
!24 = !DILocation(line: 4, column: 9, scope: !16)
!25 = !DILocation(line: 5, column: 9, scope: !26)
!26 = distinct !DILexicalBlock(scope: !16, file: !2, line: 5, column: 9)
!27 = !DILocation(line: 5, column: 9, scope: !16)
!28 = !DILocation(line: 6, column: 11, scope: !29)
!29 = distinct !DILexicalBlock(scope: !26, file: !2, line: 5, column: 15)
!30 = !DILocation(line: 7, column: 5, scope: !29)
!31 = !DILocation(line: 9, column: 28, scope: !16)
!32 = !DILocation(line: 9, column: 5, scope: !16)
!33 = !DILocation(line: 10, column: 1, scope: !16)
!34 = distinct !DISubprogram(name: "main", scope: !2, file: !2, line: 12, type: !35, scopeLine: 12, spFlags: DISPFlagDefinition, unit: !7, retainedNodes: !20)
!35 = !DISubroutineType(types: !36)
!36 = !{!19}
!37 = !DILocation(line: 13, column: 5, scope: !34)
!38 = !DILocation(line: 14, column: 5, scope: !34)
