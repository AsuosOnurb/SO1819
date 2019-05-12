import os
import random

fd = os.open("cv_in_neg_rnd_10ki.txt", os.O_CREAT | os.O_WRONLY, 0o644)

for i in range(10000):
    a = random.randrange(0, 10000)
    os.write(fd, bytes(str(a) + " -1\n", "UTF-8"))

os.close(fd)
