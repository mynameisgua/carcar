import cv2 as cv
import numpy as np



class pixalated:

    def __init__(self , img_path) -> None:
        self.img = self.read_image(img_path)
        self.gray_img = self.convert_to_grayscale(self.img)
        self.pixelated_img , self.block_size_x , self.block_size_y = self.pixelated_image(self.gray_img)
        self.pixalated_matrix , self.gray_max , self.gray_minimum= self.create_pixelated_matrix(self.pixelated_img , self.block_size_x , self.block_size_y)
        self.stretched_matrix = self.linear_stretch(self.pixalated_matrix , self.gray_max , self.gray_minimum)
        pass

    def read_image(self , img_path):
        img = cv.imread(img_path)
        #檢查
        if img is None:
            print(f"Error to read {img_path}")
            exit()
        return img
    
    def convert_to_grayscale(self , img):
        gray_img = cv.cvtColor(img, cv.COLOR_BGR2GRAY)
        return gray_img
    
    
    def pixelated_image(self , gray_img):
    
    
        #因為要創畫布,需要height,width
        height, width = gray_img.shape

        block_size_x = width // 100
        block_size_y = height // 100
    
        #創一個畫布
        pixelated_img = np.zeros((height, width), dtype = 'uint8')
    
    #------------------像素化---------------------
        for y in range(0, height, block_size_y):
            for x in range(0, width, block_size_x):
                y_end = min(y + block_size_y, height)
                x_end = min(x + block_size_x, width)
                
                block = gray_img[y:y_end, x:x_end]
                average_value = np.mean(block)
                average_value = np.uint8(average_value)
                
                pixelated_img[y:y_end, x:x_end] = average_value
    #--------------------------------------------
        return pixelated_img , block_size_x , block_size_y
    
    def create_pixelated_matrix(self , pixelated_img , block_size_x , block_size_y):
    
        #重新開一個陣列是因為pixelated_img的大小是原本的798*914
        #-----------------matrix-------------------
        height, width = pixelated_img.shape
        height_new = 100
        width_new = 100
        pixelated_matrix = np.zeros((height_new, width_new), dtype='uint8')
    
        gray_max = -1
        gray_minimum = 300
    
        for y in range(0, height_new):
            for x in range(0, width_new):
                pixelated_matrix[y, x] = pixelated_img[y*block_size_y, x*block_size_x]
                if pixelated_matrix[y , x] > gray_max:
                    gray_max = pixelated_matrix[y , x]
                if pixelated_matrix[y , x] < gray_minimum:
                    gray_minimum = pixelated_matrix[y , x]
                #print(pixelated_matrix[y, x],end=' ')
            #print()
        
        return pixelated_matrix, gray_max, gray_minimum
    
    #----------------------------------線性放大------------------
    
    def linear_stretch(self , pixelated_matrix, gray_max, gray_minimum):
        height_new, width_new = pixelated_matrix.shape
        stretched_matrix = np.zeros((height_new, width_new), dtype='uint8')
        for y in range(0 , height_new):
            for x in range(0 , width_new):
                stretched_matrix[y , x] = int(pixelated_matrix[y , x] * (255 / (gray_max - gray_minimum)) - (255 * gray_minimum / ((gray_max - gray_minimum))))
    
        return stretched_matrix
    
    
    """ def main(img_path, block_size=5):
        img = read_image(img_path)
        gray_img = convert_to_grayscale(img)
        pixelated_img = pixelated_image(gray_img, block_size)
        pixelated_matrix, gray_max, gray_minimum = create_pixelated_matrix(pixelated_img, block_size)
        
        for row in pixelated_matrix:
            print(' '.join(map(str, row)))
        
        stretched_matrix = linear_stretch(pixelated_matrix, gray_max, gray_minimum) """
        # Now you can process or save the stretched_matrix as needed
