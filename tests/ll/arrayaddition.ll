define dso_local void @_Z3addv(){
  %1 = alloca [10 x i32], align 4
  %2 = alloca [10 x i32], align 4
  %3 = alloca [10 x i32], align 4
  %4 = alloca i32, align 4
  %5 = alloca i32, align 4
  store i32 0, i32* %4, align 4
  br label %6

  %7 = load i32, i32* %4, align 4
  %8 = icmp slt i32 %7, 10
  br i1 %8, label %9, label %20

  %10 = load i32, i32* %4, align 4
  %11 = load i32, i32* %4, align 4
  %12 = getelementptr inbounds [10 x i32], [10 x i32]* %1, i32 0, i32 %11
  store i32 %10, i32* %12, align 4
  %13 = load i32, i32* %4, align 4
  %14 = sub nsw i32 %13, 1
  %15 = load i32, i32* %4, align 4
  %16 = getelementptr inbounds [10 x i32], [10 x i32]* %2, i32 0, i32 %15
  store i32 %14, i32* %16, align 4
  br label %17

  %18 = load i32, i32* %4, align 4
  %19 = add nsw i32 %18, 1
  store i32 %19, i32* %4, align 4
  br label %6

  store i32 0, i32* %5, align 4
  br label %21

  %22 = load i32, i32* %5, align 4
  %23 = icmp slt i32 %22, 10
  br i1 %23, label %24, label %37

  %25 = load i32, i32* %5, align 4
  %26 = getelementptr inbounds [10 x i32], [10 x i32]* %1, i32 0, i32 %25
  %27 = load i32, i32* %26, align 4
  %28 = load i32, i32* %5, align 4
  %29 = getelementptr inbounds [10 x i32], [10 x i32]* %2, i32 0, i32 %28
  %30 = load i32, i32* %29, align 4
  %31 = add nsw i32 %27, %30
  %32 = load i32, i32* %5, align 4
  %33 = getelementptr inbounds [10 x i32], [10 x i32]* %3, i32 0, i32 %32
  store i32 %31, i32* %33, align 4
  br label %34

  %35 = load i32, i32* %5, align 4
  %36 = add nsw i32 %35, 1
  store i32 %36, i32* %5, align 4
  br label %21

  ret void
}