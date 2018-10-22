define dso_local i32 @_Z10arithmeticiiii(i32, i32, i32, i32){
  %5 = alloca i32, align 4
  %6 = alloca i32, align 4
  %7 = alloca i32, align 4
  %8 = alloca i32, align 4
  %9 = alloca i32, align 4
  store i32 %0, i32* %5, align 4
  store i32 %1, i32* %6, align 4
  store i32 %2, i32* %7, align 4
  store i32 %3, i32* %8, align 4
  store i32 10, i32* %9, align 4
  %10 = load i32, i32* %5, align 4
  %11 = load i32, i32* %9, align 4
  %12 = add nsw i32 %11, %10
  store i32 %12, i32* %9, align 4
  %13 = load i32, i32* %6, align 4
  %14 = load i32, i32* %9, align 4
  %15 = sub nsw i32 %14, %13
  store i32 %15, i32* %9, align 4
  %16 = load i32, i32* %7, align 4
  %17 = load i32, i32* %9, align 4
  %18 = or i32 %17, %16
  store i32 %18, i32* %9, align 4
  %19 = load i32, i32* %8, align 4
  %20 = load i32, i32* %9, align 4
  %21 = and i32 %20, %19
  store i32 %21, i32* %9, align 4
  %22 = load i32, i32* %9, align 4
  ret i32 %22
}