
CONST DROHNE_SPEED=50

CONST DROHNE_OFF=0
CONST DROHNE_HIGH=1
CONST DROHNE_LOW=2

DIM Integer Drohne.W=23
DIM Integer Drohne.H=23

DIM Integer Drohne.State
DIM Float Drohne.X
DIM Float Drohne.Y
DIM Float Drohne.phi      'in DEG
DIM Float Drohne.targetX
DIM Float Drohne.targetY
DIM Float Drohne.tiAnim

sub Drohne.init()
  Drohne.state=DROHNE_OFF
end sub

sub Drohne.start(x!,y!)
  if Drohne.state=DROHNE_OFF then

    Drohne.targetX=x!
    Drohne.targetY=y!
    Drohne.State=DROHNE_HIGH
    Drohne.tiAnim=0    
  endif
end sub

sub Drohne.move(x!,y!,phi!)
  if Drohne.state=DROHNE_OFF then exit sub

  Drohne.X=x!
  Drohne.Y=y!
  Drohne.phi=phi!
end sub

sub Drohne.update()
  if Drohne.State=DROHNE_OFF then exit sub
end sub

sub Drohne.draw()
  STATIC Integer rotor=0
  LOCAL Float x,y,phi,w,h
  LOCAL Integer high
  
  if Drohne.State=DROHNE_OFF then exit sub
  if TIMER-Drohne.tiAnim>DROHNE_SPEED then Drohne.tiAnim=TIMER : rotor=rotor XOR 1
  
  x=Drohne.X : y=Drohne.Y
  if isVisible(x,y) then
    high=choice(Drohne.State=DROHNE_HIGH,0,2)
    inc x,-Drohne.W/2 : inc y,-Drohne.H/2 : phi=Drohne.phi : w=Drohne.W : h=Drohne.H
    image rotate_fast 128,(high+rotor)*h,w,h,x,y,phi,PAGE_SPRITES,1
  end if
end sub


