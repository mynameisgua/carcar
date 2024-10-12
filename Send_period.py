from pixelated import pixalated
import cv2 as cv
import numpy as np
import math

Period_amplifier = 5

gray_T = [2 , 2.5 , 2.7 , 3 , 3.5 , 5 , 7 , 8 , 10 , 11 , 12]

class PeriodString:

    def __init__(self , pixel):
        self.height , self.width =  pixel.gray_img.shape
        N = pixel.pixalated_matrix.shape[0] #number of height
        self.T_left = np.zeros(N)
        self.T_right = np.zeros(N)
        self.T_left[...] = 0
        self.T_right[...] = 0
        self.Period_numbers = 0
        self.Period_index = 0
        self.canvas_width = pixel.block_size_x * 100
    
    def convert_to_string(self , row_index , pixel):
        period_string = "|"
        left_pos = 0
        right_pos = 0
        Period_number = 0
        while right_pos < self.canvas_width:
            self.T_left[row_index] = pixel.pixalated_matrix[row_index , int(left_pos // pixel.block_size_x)] // 25
            self.T_right[row_index] = pixel.pixalated_matrix[row_index , int(right_pos // pixel.block_size_x)] // 25
            Period = gray_T[int(self.T_left[row_index])]
            right_pos += math.pi * Period / Period_amplifier
            if self.T_left[row_index] != self.T_right[row_index]:
                left_pos = right_pos
                period_string += str(Period_number) + "," + str(int(self.T_left[row_index])) + "|"
                Period_number = 0
            else:
                Period_number += 1
        period_string += str(Period_number) + "," + str(int(self.T_left[row_index])) + "|"
        return period_string
