import matplotlib.pyplot as plt
import numpy as np

class Chunk:
    x: int
    y: int

    def __init__(self, x, y):
        self.loaded = False
        self.x = x
        self.y = y

class ArrayCircular2D:
    data: list[list[Chunk]]
    size: int
    half: int
    current_x: int
    current_y: int

    def __init__(self, half):
        self.half = half
        self.size = 2*half+ 1
        self.current_x = 0
        self.current_y = 0
        self.reset_data()

    def get_id_from_indices(self, x, y):
        x_abs, y_abs = self.get_indices_from_id(self.current_x, self.current_y)

        dx = (x - x_abs) % self.size
        dy = (y - y_abs) % self.size
        if dx > self.half:
            dx -= self.size
        if dy > self.half:
            dy -= self.size

        return (self.current_x + dx, self.current_y + dy)

    def get_indices_from_id(self, x, y):
        return (x % self.size, y % self.size)

    def reset_data(self):
        self.data = [[Chunk(*self.get_id_from_indices(x, y)) for y in range(self.size)] for x in range(self.size)]


    def move(self, x_rel, y_rel):
        self.current_x += x_rel
        self.current_y += y_rel

        if abs(x_rel) >= self.size or abs(y_rel) >= self.size:
            print("RESET")
            self.reset_data()
            return


        if x_rel >= 0:
            r = range(-self.half, x_rel-self.half)
        else:
            r = range(self.half + x_rel + 1, self.half + 1)

        for i in r:
            for j in range(self.size):
                x, y = self.get_indices_from_id(self.current_x - x_rel + i, self.current_y + j)
                self.data[x][y] = Chunk(*self.get_id_from_indices(x, y))


        if y_rel >= 0:
            r = range(-self.half, y_rel-self.half)
        else:
            r = range(self.half + y_rel + 1, self.half + 1)

        for i in range(self.size):
            for j in r:
                x, y = self.get_indices_from_id(self.current_x + i, self.current_y - y_rel + j)
                self.data[x][y] = Chunk(*self.get_id_from_indices(x, y))

def showCirc(m, circ, c):
     half = 2*circ.size
     for xs in circ.data:
         for chunk in xs:
             m[chunk.x + half][chunk.y + half] = c

     m[circ.current_x + half][circ.current_y + half] = c*1.2

def main():
     circ = ArrayCircular2D(2)

     m = np.zeros((4*circ.size+1, 4*circ.size+1))
     showCirc(m, circ, 0.6)

     circ.move(-4, -2)

     showCirc(m, circ, 0.4)


     plt.imshow(m, cmap='Greys')
     plt.show()

if __name__ == "__main__":
    main()

