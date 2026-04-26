#TRUE# = ^t.^f.t
#FALSE# = ^t.^f.f

#ISZERO# = ^n.((n ^x.#FALSE#) #TRUE#)
#ADD# = ^m.^n.^f.^x.((m f) ((n f) x))

#PAIR# = ^x.^y.^f.((f x) y)
#FST# = ^p.(p #TRUE#)
#SND# = ^p.(p #FALSE#)

#NEXT_PRED# = ^p.((#PAIR# (#SND# p)) ((#ADD# 1) (#SND# p)))
#PRED# = ^n.(#FST# ((n #NEXT_PRED#) ((#PAIR# 0) 0)))

#Y# = ^f.(^x.(f (x x)) ^x.(f (x x)))

#FIB_REC# = ^f.^n.(((#ISZERO# n) 0) (((#ISZERO# (#PRED# n)) 1) ((#ADD# (f (#PRED# n))) (f (#PRED# (#PRED# n))))))
#FIB# = (#Y# #FIB_REC#)

(#FIB# 7)
