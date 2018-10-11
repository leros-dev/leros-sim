; Logical shift left by immediate
define dso_local i32 @_Z4shlii(i32) {
  %2 = alloca i32, align 4
  store i32 %0, i32* %2, align 4
  %3 = load i32, i32* %2, align 4
  %4 = shl i32 %3, 5
  ret i32 %4
}

; Logical shift left by register
define dso_local i32 @_Z4shlrii(i32, i32)  {
  %3 = alloca i32, align 4
  %4 = alloca i32, align 4
  store i32 %0, i32* %3, align 4
  store i32 %1, i32* %4, align 4
  %5 = load i32, i32* %3, align 4
  %6 = load i32, i32* %4, align 4
  %7 = shl i32 %5, %6
  ret i32 %7
}

; logical shift right by register
define dso_local i32 @_Z5lshrrjj(i32, i32) {
  %3 = alloca i32, align 4
  %4 = alloca i32, align 4
  store i32 %0, i32* %3, align 4
  store i32 %1, i32* %4, align 4
  %5 = load i32, i32* %3, align 4
  %6 = load i32, i32* %4, align 4
  %7 = lshr i32 %5, %6
  ret i32 %7
}


; logical shift right by immediate
define dso_local i32 @_Z5lshrij(i32) {
  %2 = alloca i32, align 4
  store i32 %0, i32* %2, align 4
  %3 = load i32, i32* %2, align 4
  %4 = lshr i32 %3, 5
  ret i32 %4
}

; arithmetic shift right by register
define dso_local i32 @_Z5lshrrii(i32, i32) {
  %3 = alloca i32, align 4
  %4 = alloca i32, align 4
  store i32 %0, i32* %3, align 4
  store i32 %1, i32* %4, align 4
  %5 = load i32, i32* %3, align 4
  %6 = load i32, i32* %4, align 4
  %7 = ashr i32 %5, %6
  ret i32 %7
}

; arithmetic shift right by immediate
define dso_local i32 @_Z5lshrii(i32)  {
  %2 = alloca i32, align 4
  store i32 %0, i32* %2, align 4
  %3 = load i32, i32* %2, align 4
  %4 = ashr i32 %3, 5
  ret i32 %4
}