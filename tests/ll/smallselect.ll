define dso_local i32 @_Z3fooi(i32)  {
  %2 = icmp sgt i32 %0, 0
  %3 = select i1 %2, i32 %0, i32 0
  ret i32 %3
}