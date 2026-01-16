; ModuleID = '../test/input.c'
source_filename = "../test/input.c"
target datalayout = "e-m:o-i64:64-i128:128-n32:64-S128"
target triple = "arm64-apple-macosx16.0.0"

@.str = private unnamed_addr constant [11 x i8] c"Value: %d\0A\00", align 1
@str = private unnamed_addr constant [12 x i8] c"Starting...\00", align 1

; Function Attrs: nofree nounwind ssp uwtable
define void @sensitive_calculation(i32 noundef %0) local_unnamed_addr #0 {
  %2 = alloca [10 x i32], align 4
  call void @llvm.lifetime.start.p0(i64 40, ptr nonnull %2) #4
  %3 = sext i32 %0 to i64
  %4 = getelementptr inbounds [10 x i32], ptr %2, i64 0, i64 %3
  store volatile i32 42, ptr %4, align 4, !tbaa !5
  %5 = load volatile i32, ptr %4, align 4, !tbaa !5
  %6 = tail call i32 (ptr, ...) @printf(ptr noundef nonnull @.str, i32 noundef %5)
  call void @llvm.lifetime.end.p0(i64 40, ptr nonnull %2) #4
  ret void
}

; Function Attrs: argmemonly mustprogress nocallback nofree nosync nounwind willreturn
declare void @llvm.lifetime.start.p0(i64 immarg, ptr nocapture) #1

; Function Attrs: nofree nounwind
declare noundef i32 @printf(ptr nocapture noundef readonly, ...) local_unnamed_addr #2

; Function Attrs: argmemonly mustprogress nocallback nofree nosync nounwind willreturn
declare void @llvm.lifetime.end.p0(i64 immarg, ptr nocapture) #1

; Function Attrs: nofree nounwind ssp uwtable
define i32 @main() local_unnamed_addr #0 {
  %1 = alloca i32, align 4
  %2 = tail call i32 @puts(ptr nonnull @str)
  call void @llvm.lifetime.start.p0(i64 4, ptr nonnull %1)
  store volatile i32 42, ptr %1, align 4, !tbaa !5
  %3 = load volatile i32, ptr %1, align 4, !tbaa !5
  %4 = tail call i32 (ptr, ...) @printf(ptr noundef nonnull @.str, i32 noundef %3)
  call void @llvm.lifetime.end.p0(i64 4, ptr nonnull %1)
  ret i32 0
}

; Function Attrs: nofree nounwind
declare noundef i32 @puts(ptr nocapture noundef readonly) local_unnamed_addr #3

attributes #0 = { nofree nounwind ssp uwtable "frame-pointer"="non-leaf" "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="apple-m1" "target-features"="+aes,+crc,+crypto,+dotprod,+fp-armv8,+fp16fml,+fullfp16,+lse,+neon,+ras,+rcpc,+rdm,+sha2,+sha3,+sm4,+v8.5a,+zcm,+zcz" }
attributes #1 = { argmemonly mustprogress nocallback nofree nosync nounwind willreturn }
attributes #2 = { nofree nounwind "frame-pointer"="non-leaf" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="apple-m1" "target-features"="+aes,+crc,+crypto,+dotprod,+fp-armv8,+fp16fml,+fullfp16,+lse,+neon,+ras,+rcpc,+rdm,+sha2,+sha3,+sm4,+v8.5a,+zcm,+zcz" }
attributes #3 = { nofree nounwind }
attributes #4 = { nounwind }

!llvm.module.flags = !{!0, !1, !2, !3}
!llvm.ident = !{!4}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{i32 7, !"PIC Level", i32 2}
!2 = !{i32 7, !"uwtable", i32 2}
!3 = !{i32 7, !"frame-pointer", i32 1}
!4 = !{!"Homebrew clang version 15.0.7"}
!5 = !{!6, !6, i64 0}
!6 = !{!"int", !7, i64 0}
!7 = !{!"omnipotent char", !8, i64 0}
!8 = !{!"Simple C/C++ TBAA"}
