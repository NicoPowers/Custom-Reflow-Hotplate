import matplotlib.pyplot as plt
import matplotlib.animation as animation
from time import sleep
import csv
import serial

#initialize serial port
ser = serial.Serial()
ser.port = '/dev/ttyACM0' #Arduino serial port
ser.baudrate = 115200
ser.timeout = 10 #specify timeout when using readline()
ser.open()
if ser.is_open==True:
	print("\nSerial port now open. Configuration:\n")
	print(ser, "\n") #print serial parameters

ser.flushInput()


# Create figure for plotting
fig = plt.figure()
ax = fig.add_subplot(1, 1, 1)
timeDataPoints = set() 
tempDataPoints = [] 

# stuff for csv
header = ["time", "temperature"]
data = []

# This function is called periodically from FuncAnimation
def animate(time, timeDataPoints, tempDataPoints):

    #Aquire and parse data from serial port
    if (ser.in_waiting):
        line=ser.readline()      #ascii
        print(line)        
        line_as_list = line.split(b',')            
        if (len(line_as_list) == 2):
            try:                       
                time = float(line_as_list[0])
                if time not in timeDataPoints:            
                    temp = line_as_list[1]
                    temp_as_list = temp.split(b'\n')
                    temp_float = float(temp_as_list[0])
                    
                    # Add x and y to lists
                    timeDataPoints.add(time)
                    tempDataPoints.append(temp_float)

                    data.append([time, temp_float])                        

                    # Draw x and y lists
                    ax.clear()
                    ax.plot(sorted(timeDataPoints), tempDataPoints, label="Temperature")                

                    plt.title('Temperature vs. Time')
                    plt.ylabel('Temperature')
                    plt.xlabel('Time')
                    plt.legend()
            except Exception as e:
                print("Data transmission error.")                    


try:
    while(ser.in_waiting):
        ser.readline()
        
    # Set up plot to call animate() function periodically
    ani = animation.FuncAnimation(fig, animate, fargs=(timeDataPoints, tempDataPoints), interval=500)
    plt.show()
    
except KeyboardInterrupt:
    print("Canceled by user")

finally:
    # save data to csv
    with open('plot_data.csv', 'w') as f:
        writer = csv.writer(f)

        # write the header
        writer.writerow(header)

        # write multiple rows
        writer.writerows(data)

