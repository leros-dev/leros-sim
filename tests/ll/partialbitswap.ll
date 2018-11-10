define dso_local i32 @_Z3fooiii(i32, i32, i32) {
  %4 = alloca i32, align 4
  %5 = alloca i32, align 4
  %6 = alloca i32, align 4
  %7 = alloca i32, align 4
  %8 = alloca i32, align 4
  %9 = alloca i32, align 4
  store i32 %0, i32* %4, align 4
  
  store i32 %1, i32* %5, align 4
  
  store i32 %2, i32* %6, align 4
  
  
  %10 = load i32, i32* %4, align 4
  %11 = load i32, i32* %5, align 4
  %12 = ashr i32 %10, %11
  %13 = and i32 %12, 1
  store i32 %13, i32* %7, align 4
  
  %14 = load i32, i32* %4, align 4
  %15 = load i32, i32* %6, align 4
  %16 = ashr i32 %14, %15
  %17 = and i32 %16, 1
  store i32 %17, i32* %8, align 4
  
  %18 = load i32, i32* %7, align 4
  %19 = load i32, i32* %8, align 4
  %20 = xor i32 %18, %19
  store i32 %20, i32* %9, align 4
  %21 = load i32, i32* %9, align 4
  %22 = load i32, i32* %5, align 4
  %23 = shl i32 %21, %22
  %24 = load i32, i32* %9, align 4
  %25 = load i32, i32* %6, align 4
  %26 = shl i32 %24, %25
  %27 = or i32 %23, %26
  store i32 %27, i32* %9, align 4
  %28 = load i32, i32* %9, align 4
  ret i32 %28
}