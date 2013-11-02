OBJECTS = monstrosity.o
TARGET = monstrosity
CFLAGS = -std=gnu99 -Wall
LDFLAGS = -lusb-1.0 

all: monster

monster: $(OBJECTS)
	$(CC) $(CFLAGS) $(LDFLAGS) $(OBJECTS) -o $(@)

%.o: %.c
	$(CC) $(CFLAGS) -c $(<) -o $(@)    

clean:
	rm -rf $(TARGET) $(OBJECTS)
