define dso_local void @_Z3fooPi(i32*){
  %2 = alloca i32*, align 4
  store i32* %0, i32** %2, align 4
  %3 = load i32*, i32** %2, align 4
  store i32 1, i32* %3, align 4
  %4 = load i32*, i32** %2, align 4
  %5 = getelementptr inbounds i32, i32* %4, i32 1
  store i32 2, i32* %5, align 4
  %6 = load i32*, i32** %2, align 4
  %7 = getelementptr inbounds i32, i32* %6, i32 2
  store i32 3, i32* %7, align 4
  ret void
}