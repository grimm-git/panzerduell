

DIM String Settings.name

sub Settings.load(name$)
  LOCAL String line$,key$,num$
  LOCAL Integer n,value

  Settings.name=name$

  on error skip 1
  open Settings.name for INPUT as #1
  if MM.ERRNO>0 then print MM.ERRMSG$ : exit sub

  do
    line input #1,line$
    if line$="" then exit do

    key$=field$(line$,1,",")

    select case key
    case "keyset_0"
      for n=0 to 5
        num$=field$(line$,n+2,",")
        Controls.keys(0,n)=val(num$)
      next    
    case "keyset_1"
      for n=0 to 5
        num$=field$(line$,n+2,",")
        Controls.keys(1,n)=val(num$)
      next    
    end select
  loop

  close #1
end sub

sub Settings.save()
  LOCAL Integer n,m

  open Settings.name for OUTPUT as #1

  for m=0 to 1
    print #1,"keyset_"+str$(m);
    for n=0 to 5
      print #1,","+str$(Controls.keys(m,n));
    next
    print #1
  next

  close #1
end sub
