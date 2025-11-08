
CONST SHOTS_MAX=40
CONST SHOTS_SPEED=2

DIM Integer Shots.next
DIM Float   Shots.X(SHOTS_MAX) 
DIM Float   Shots.Y(SHOTS_MAX)
DIM Float   Shots.dX(SHOTS_MAX)
DIM Float   Shots.dY(SHOTS_MAX)
DIM Integer Shots.owner(SHOTS_MAX)

sub Shots.init()
  Shots.next=0
end sub

sub Shots.fire(id%,x!,y!,angle!)
  LOCAL Float sinphi=sin(angle!/180*PI)
  LOCAL Float cosphi=cos(angle!/180*PI)

  if Shots.next>SHOTS_MAX then exit sub

  Shots.X(Shots.next)=x!+12*sinphi
  Shots.dX(Shots.next)=SHOTS_SPEED*sinphi
  Shots.Y(Shots.next)=y!-12*cosphi
  Shots.dY(Shots.next)=-SHOTS_SPEED*cosphi
  Shots.owner(Shots.next)=id%
  PlaySample 6,22050
  inc Shots.next
end sub

sub Shots.update()
  LOCAL Float x,y
  LOCAL Integer s,hit

  for s=0 to Shots.next-1
    inc Shots.X(s),cfr(Shots.dX(s))
    inc Shots.Y(s),cfr(Shots.dY(s))

    x=Shots.X(s) : y=Shots.Y(s)
    hit=Map.hitByBullet(x,y)
    if Shots.hitWall(x,y) or hit=1 then
      Emitter.shot(x,y)
      playSample 15,22050
      Shots.terminate(s)
    elseif hit=2 then
      'hit reaction has already been taken care for
      Shots.terminate(s)
    elseif isVisible(x,y) then
      pixel x,y,map(WHITE)
      pixel x,y-1,map(WHITE)
      pixel x+1,y,map(WHITE)
      pixel x+1,y-1,map(WHITE)
    else
      Shots.terminate(s)
    endif

    hit=Panzer.isHit(x,y)
    if hit>=0 then
      if hit=Shots.owner(s) then continue for
      if Panzer.isActive(hit) then Player.incKills(Shots.owner(s)+1) : Player.kill(hit+1)
      if Not Panzer.isRepair(hit) then Panzer.destroy hit
      Shots.terminate(s)
      continue for
    endif
  next
end sub

function Shots.hitWall(x!,y!) as Integer
  if x!<Playfield.X-6 or x!>Playfield.X+Playfield.W+6 then goto out
  if y!<Playfield.Y-10 or y!>Playfield.Y+Playfield.H+10 then goto out
  exit function
out:
  Shots.hitWall=1
end function

sub Shots.terminate(s%)
  _copyShots Shots.next-1,s%
  inc Shots.next,-1
end sub

sub _copyShots(idxFrom%,idxTo%)
  if idxFrom%=idxTo% then exit sub

  Shots.X(idxTo%)=Shots.X(idxFrom%)
  Shots.Y(idxTo%)=Shots.Y(idxFrom%)
  Shots.dX(idxTo%)=Shots.dX(idxFrom%)
  Shots.dY(idxTo%)=Shots.dY(idxFrom%)
  Shots.owner(idxTo%)=Shots.owner(idxFrom%)
end sub



