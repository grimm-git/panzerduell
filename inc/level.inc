
CONST LEVEL_2P=2
CONST LEVEL_3P=0
CONST LEVEL_4P=0

sub Level.load(num%)
  LOCAL Integer lvl
  LOCAL String tmp$,cmd$
  LOCAL Integer arg1,arg2,arg3,arg4

  select case num%  
  case 1,2
    lvl=int(RND()*LEVEL_2P+1)
    tmp$="P2_"+str$(lvl,2,0,"0")
    Level.setTwoPanzer
    if num%=1 then Player.setRobot(2)
  case 3
    lvl=int(RND()*LEVEL_3P+1)
    tmp$="P3_"+str$(lvl,2,0,"0")
    Level.setThreePanzer
  case 4
    lvl=int(RND()*LEVEL_4P+1)
    tmp$="P4_"+str$(lvl,2,0,"0")
    Level.setFourPanzer
  end select
  restore tmp$

  read tmp$
  Map.load "lvl/"+tmp$+".map"

  do while cmd$<>"END"
    read cmd$
    select case cmd$
      case "O":  'Object
        read arg1,arg2,arg3,arg4
        Map.addObject(arg1,arg2,arg3,arg4)
    end select
  loop
end sub

sub Level.setTwoPanzer()
  LOCAL Float x,y,w,h

  x=Playfield.X : y=Playfield.Y : w=Playfield.W : h=Playfield.H
  Panzer.set(0,x  +75,y+h/2, 90)
  Panzer.set(1,x+w-75,y+h/2,-90)
end sub

sub Level.setThreePanzer()
  LOCAL Float x,y,w,h

  x=Playfield.X : y=Playfield.Y : w=Playfield.W : h=Playfield.H
  Panzer.set(0,x  +75,y+h/2 ,  90)
  Panzer.set(1,x+w-75,y+  75,-120)
  Panzer.set(2,x+w-75,y+h-75, -60)
end sub

sub Level.setFourPanzer()
  LOCAL Float x,y,w,h

  x=Playfield.X : y=Playfield.Y : w=Playfield.W : h=Playfield.H
  Panzer.set(0,x  +75,y  +75, 120)
  Panzer.set(1,x  +75,y+h-75,  60)
  Panzer.set(2,x+w-75,y  +75,-120)
  Panzer.set(3,x+w-75,y+h-75, -60)
end sub

P2_01:
DATA "P2_01"
DATA "O",150,66,35,71
DATA "O",64,245,42,38
DATA "O",280,1,32,43
DATA "O",280,135,32,65
DATA "O",280,293,32,43
DATA "O",408,199,35,71
DATA "O",486,53,42,38
DATA "END"

P2_02:
DATA "P2_02"
DATA "O",54,48,37,43
DATA "O",54,245,37,43
DATA "O",210,152,39,32
DATA "O",280,0,32,43
DATA "O",280,98,32,39
DATA "O",280,199,32,39
DATA "O",280,293,32,43
DATA "O",343,152,39,32
DATA "O",503,48,37,43
DATA "O",503,246,37,43
DATA "END"


