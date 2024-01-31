start with pwhash (p), enc_salt (s), and last_block_hash (b), data (d), hash_function (h), actual encrtyption salt (enc)
known to attacker (k), unknown to attacker (u)

p = got from the password (u)
b = zero string (k)
s = gotten from the header (k)
d = data that is encrypted (k, because the attacker could have get onto the data and the algo has to be secure)

p = h(p(u) + s(k) + b(k)) (u)
s = h(p(u) + s(k) + b(k)) (u)
enc = h(p(u) + s(u)) (k) -> enc is actually known (because enc_d and d is known)

loop
b = h(d(k)) (k)
enc_d = d(k) + enc(k) (k)

p = h(p(u) + s(u) + b(k)) (u)
s = h(p(u) + s(u) + b(k)) (u)
enc = h(p(u) + s(u)) (k) -> enc is actually known (because enc_d and d is known)

Better:
s = h(p(u) + s(k)) (u)
loop
p = h(p(u) + s(u) + b(k)) (u)
enc = h(p(u) + s(u)) (k) -> enc is actually known (because enc_d and d is known)

b = h(d(k)) (k)
enc_d = d(k) + enc(k) (k)

