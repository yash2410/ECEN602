import pandas as pd
import matplotlib.pyplot as plt

df = pd.read_csv("tr.csv")
x = df["time"]
th1 = df["th1"]
th2 = df["th2"]

plt.plot(x,th1,"-b",x,th2,"-r")
plt.xlabel("Time in Seconds")
plt.ylabel("Throughput in Mbps")
plt.legend(["th1","th2"])
plt.show()
