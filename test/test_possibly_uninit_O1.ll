; ModuleID = 'test_possibly_uninit.c'
source_filename = "test_possibly_uninit.c"
target datalayout = "e-m:o-i64:64-i128:128-n32:64-S128"
target triple = "arm64-apple-macosx16.0.0"

@.str = private unnamed_addr constant [12 x i8] c"Result: %d\0A\00", align 1, !dbg !0

; Function Attrs: nofree nounwind ssp uwtable
define void @conditional_init(i32 noundef %0) local_unnamed_addr #0 !dbg !16 {
  call void @llvm.dbg.value(metadata i32 %0, metadata !21, metadata !DIExpression()), !dbg !23
  %2 = tail call i32 (ptr, ...) @printf(ptr noundef nonnull @.str, i32 noundef 5), !dbg !24
  ret void, !dbg !25
}

; Function Attrs: nofree nounwind
declare noundef i32 @printf(ptr nocapture noundef readonly, ...) local_unnamed_addr #1

; Function Attrs: nofree nounwind ssp uwtable
define i32 @main() local_unnamed_addr #0 !dbg !26 {
  call void @llvm.dbg.value(metadata i32 1, metadata !21, metadata !DIExpression()), !dbg !30
  %1 = tail call i32 (ptr, ...) @printf(ptr noundef nonnull @.str, i32 noundef 5), !dbg !32
  ret i32 0, !dbg !33
}

; Function Attrs: nocallback nofree nosync nounwind readnone speculatable willreturn
declare void @llvm.dbg.value(metadata, metadata, metadata) #2

attributes #0 = { nofree nounwind ssp uwtable "frame-pointer"="non-leaf" "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="apple-m1" "target-features"="+aes,+crc,+crypto,+dotprod,+fp-armv8,+fp16fml,+fullfp16,+lse,+neon,+ras,+rcpc,+rdm,+sha2,+sha3,+sm4,+v8.5a,+zcm,+zcz" }
attributes #1 = { nofree nounwind "frame-pointer"="non-leaf" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="apple-m1" "target-features"="+aes,+crc,+crypto,+dotprod,+fp-armv8,+fp16fml,+fullfp16,+lse,+neon,+ras,+rcpc,+rdm,+sha2,+sha3,+sm4,+v8.5a,+zcm,+zcz" }
attributes #2 = { nocallback nofree nosync nounwind readnone speculatable willreturn }

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
!7 = distinct !DICompileUnit(language: DW_LANG_C99, file: !2, producer: "Homebrew clang version 15.0.7", isOptimized: true, runtimeVersion: 0, emissionKind: FullDebug, globals: !8, splitDebugInlining: false, nameTableKind: None, sysroot: "/Library/Developer/CommandLineTools/SDKs/MacOSX14.sdk", sdk: "MacOSX14.sdk")
!8 = !{!0}
!9 = !{i32 7, !"Dwarf Version", i32 4}
!10 = !{i32 2, !"Debug Info Version", i32 3}
!11 = !{i32 1, !"wchar_size", i32 4}
!12 = !{i32 7, !"PIC Level", i32 2}
!13 = !{i32 7, !"uwtable", i32 2}
!14 = !{i32 7, !"frame-pointer", i32 1}
!15 = !{!"Homebrew clang version 15.0.7"}
!16 = distinct !DISubprogram(name: "conditional_init", scope: !2, file: !2, line: 3, type: !17, scopeLine: 3, flags: DIFlagPrototyped | DIFlagAllCallsDescribed, spFlags: DISPFlagDefinition | DISPFlagOptimized, unit: !7, retainedNodes: !20)
!17 = !DISubroutineType(types: !18)
!18 = !{null, !19}
!19 = !DIBasicType(name: "int", size: 32, encoding: DW_ATE_signed)
!20 = !{!21, !22}
!21 = !DILocalVariable(name: "flag", arg: 1, scope: !16, file: !2, line: 3, type: !19)
!22 = !DILocalVariable(name: "x", scope: !16, file: !2, line: 4, type: !19)
!23 = !DILocation(line: 0, scope: !16)
!24 = !DILocation(line: 9, column: 5, scope: !16)
!25 = !DILocation(line: 10, column: 1, scope: !16)
!26 = distinct !DISubprogram(name: "main", scope: !2, file: !2, line: 12, type: !27, scopeLine: 12, flags: DIFlagAllCallsDescribed, spFlags: DISPFlagDefinition | DISPFlagOptimized, unit: !7, retainedNodes: !29)
!27 = !DISubroutineType(types: !28)
!28 = !{!19}
!29 = !{}
!30 = !DILocation(line: 0, scope: !16, inlinedAt: !31)
!31 = distinct !DILocation(line: 13, column: 5, scope: !26)
!32 = !DILocation(line: 9, column: 5, scope: !16, inlinedAt: !31)
!33 = !DILocation(line: 14, column: 5, scope: !26)
