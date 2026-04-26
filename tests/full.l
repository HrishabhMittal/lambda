#TRUE# = ^t.^f.t
#FALSE# = ^t.^f.f

#NOT# = ^b.((b #FALSE#) #TRUE#)
#AND# = ^p.^q.((p q) p)
#OR# = ^p.^q.((p p) q)

#SUCC# = ^n.^f.^x.(f ((n f) x))
#ADD# = ^m.^n.^f.^x.((m f) ((n f) x))
#MUL# = ^m.^n.^f.^x.((m (n f)) x)
#POW# = ^b.^e.(e b)
#ISZERO# = ^n.((n ^x.#FALSE#) #TRUE#)

#PAIR# = ^x.^y.^f.((f x) y)
#FST# = ^p.(p #TRUE#)
#SND# = ^p.(p #FALSE#)

(#NOT# #TRUE#)

((#AND# #TRUE#) #FALSE#)

((#OR# #FALSE#) #TRUE#)

((#ADD# 2) 3)

((#MUL# 2) 3)

((#POW# 2) 3)

(#FST# ((#PAIR# a) b))

(#SND# ((#PAIR# a) b))

(#ISZERO# 0)

(#ISZERO# 3)

(^x.^y.(x y) (a b))
