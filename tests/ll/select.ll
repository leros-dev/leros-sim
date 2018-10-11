define dso_local i32 @_Z2f1ii(i32, i32)  {
  %3 = icmp sgt i32 %0, %1
  %4 = select i1 %3, i32 %0, i32 %1
  ret i32 %4
}

define dso_local i32 @_Z2f2ii(i32, i32)  {
  %3 = icmp sge i32 %0, %1
  %4 = select i1 %3, i32 %1, i32 %0
  ret i32 %4
}

define dso_local i32 @_Z2f3ii(i32, i32)  {
  %3 = icmp slt i32 %0, %1
  %4 = select i1 %3, i32 %0, i32 %1
  ret i32 %4
}

define dso_local i32 @_Z2f4ii(i32, i32) {
  %3 = icmp sle i32 %0, %1
  %4 = select i1 %3, i32 %1, i32 %0
  ret i32 %4
}

define dso_local i32 @_Z2f5ii(i32, i32) {
  %3 = icmp eq i32 %0, %1
  %4 = select i1 %3, i32 %0, i32 0
  ret i32 %4
}