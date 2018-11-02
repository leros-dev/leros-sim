define dso_local i32 @arrayaddition(i32) {
  %2 = alloca i32, i32 %0, align 4
  %3 = alloca i32, i32 %0, align 4
  %4 = alloca i32, i32 %0, align 4
  %5 = icmp sgt i32 %0, 0
  br i1 %5, label %7, label %25
  br i1 %5, label %15, label %25
  %8 = phi i32 [ %12, %7 ], [ 0, %1 ]
  %9 = getelementptr inbounds i32, i32* %2, i32 %8
  store i32 %8, i32* %9, align 4
  %10 = add nsw i32 %8, -1
  %11 = getelementptr inbounds i32, i32* %3, i32 %8
  store i32 %10, i32* %11, align 4
  %12 = add nuw nsw i32 %8, 1
  %13 = icmp eq i32 %12, %0
  br i1 %13, label %6, label %7
  br i1 %5, label %27, label %25
  %16 = phi i32 [ %23, %15 ], [ 0, %6 ]
  %17 = getelementptr inbounds i32, i32* %2, i32 %16
  %18 = load i32, i32* %17, align 4
  %19 = getelementptr inbounds i32, i32* %3, i32 %16
  %20 = load i32, i32* %19, align 4
  %21 = add nsw i32 %20, %18
  %22 = getelementptr inbounds i32, i32* %4, i32 %16
  store i32 %21, i32* %22, align 4
  %23 = add nuw nsw i32 %16, 1
  %24 = icmp eq i32 %23, %0
  br i1 %24, label %14, label %15
  %26 = phi i32 [ 0, %14 ], [ 0, %6 ], [ 0, %1 ], [ %32, %27 ]
  ret i32 %26
  %28 = phi i32 [ %33, %27 ], [ 0, %14 ]
  %29 = phi i32 [ %32, %27 ], [ 0, %14 ]
  %30 = getelementptr inbounds i32, i32* %4, i32 %28
  %31 = load i32, i32* %30, align 4
  %32 = add nsw i32 %31, %29
  %33 = add nuw nsw i32 %28, 1
  %34 = icmp eq i32 %33, %0
  br i1 %34, label %25, label %27
}