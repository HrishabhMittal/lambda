#TRUE# = ^t.^f.t
#FALSE# = ^t.^f.f
#ADD# = ^m.^n.^f.^x.((m f) ((n f) x))

#PAIR# = ^x.^y.^f.((f x) y)
#FST# = ^p.(p #TRUE#)
#SND# = ^p.(p #FALSE#)

#FIB_STEP# = ^p.((#PAIR# (#SND# p)) ((#ADD# (#FST# p)) (#SND# p)))
#FIB# = ^n.(#FST# ((n #FIB_STEP#) ((#PAIR# 0) 1)))

(#FIB# 10)
