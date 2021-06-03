import matplotlib.pyplot as plt
import numpy as np

distance=[]
cdf=[]

with open("missdistFrequency4.txt","rt") as file:
    for line in file:
        tmp=line.split(" ")
        distance.append(np.log10(int(tmp[0])))
        cdf.append(float(tmp[1]))

plt.xlabel("log-to-the-base-10 (AccessDistance)")
plt.ylabel("Cummulative Density")
plt.plot(distance,cdf)
plt.show()


file.close()
