#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Function that writes the given matrix to the file of the desired name */
void create_PGM_file (int **image, int col, int row, int maxVal, char *filename){
	int i, j;
	FILE *fp;
	
	if((fp = fopen(filename, "wb")) == NULL){
        printf("Error opening file!");
        return;
    }
    
	fprintf(fp, "P5\n");
    fprintf(fp, "%d %d\n", col, row);
    fprintf(fp, "%d\n", maxVal);
    
    for(i = 0; i < row; i++)
        for(j = 0; j < col; j++)
            fputc(image[i][j], fp);
    
	fclose(fp);	
}

/* Function that creates a matrix with the given number of rows and columns */
int** create_matrix (int row, int col){
    int **matrix;
    int i;
    
    matrix = (int**)calloc(row,sizeof(int*));
    for( i = 0 ; i < row ; i++)
        matrix[i] = (int*)calloc(col,sizeof(int));
    
	return matrix;
}

/* Function that applies mean filter on image with given kernel size */
void mean_filter (int **image, int row, int col, int **filtered_image, int kernel_size){
	int i, j;
	int r, c, sum;
	int masksize = kernel_size / 2;
	
	for (r = masksize; r < row - masksize; r++){
		for (c = masksize; c< col - masksize; c++){
			sum = 0;
			for(i = 0; i<kernel_size; i++){
				for(j = 0; j<kernel_size; j++){
					sum += image[r+(masksize)-i][c+masksize-j];
				}
		   }
		   filtered_image[r][c] = (int)sum / (kernel_size*kernel_size);
       }
	}
	
	//The parts of the image that are not included in the filtering process are copied from the original image
	for(i=0; i< masksize; i++){
		for(j=0; j<col; j++){
			filtered_image[i][j] = image[i][j];
			filtered_image[row-1-i][j] = image[row-1-i][j];
		}
	}
	for(i=0; i< masksize; i++){
		for(j=0; j<row; j++){
			filtered_image[j][i] = image[j][i];
			filtered_image[j][col-1-i] = image[j][col-1-i];
		}
	}
}

/* Insertion sort function used in median filter */
void insertion_sort (int *arr, int n) {
    int i, j, tmp;
    for (i = 1; i < n; i++) {
        tmp = arr[i];
        j = i - 1;
        
        while (j >= 0 && arr[j] > tmp) {
            arr[j + 1] = arr[j];
            j = j - 1;
        }
        arr[j + 1] = tmp;
    }
}

/* Function that applies the median filter on the given image */
void median_filter (int **image, int row, int col, int **filtered_image, int window_size){
	int i, j, k, *window;
	int r,c;
    int masksize = window_size / 2;
    
    window = (int*) malloc((window_size*window_size)*sizeof(int));
    
	for(r = masksize; r < row - masksize; ++r) {
        for(c = masksize; c < col - masksize; ++c) {
            k=0;
			for(i = 0; i<window_size; i++){
				for(j = 0; j<window_size; j++){
					window[k] = image[r+masksize-i][c+masksize-j];
					k++;
				}
		   }
            //Sort the pixels in the window to find the median
            insertion_sort(window, (window_size*window_size));
            //The median value is assigned to the new image
            filtered_image[r][c]=window[(window_size*window_size)/2];
        }
    }
    
    //The parts of the image that are not included in the filtering process are copied from the original image
	for(i=0; i< masksize; i++){
		for(j=0; j<col; j++){
			filtered_image[i][j] = image[i][j];
			filtered_image[row-1-i][j] = image[row-1-i][j];
		}
	}
	for(i=0; i< masksize; i++){
		for(j=0; j<row; j++){
			filtered_image[j][i] = image[j][i];
			filtered_image[j][col-1-i] = image[j][col-1-i];
		}
	}
}

int main(){
    char line[2000];				//Used to read data from file
    char filename[50];				//Stores the name of the image to open
    char savename[50];				//Stores the name of the image to be saved
	FILE *fp;						//File pointer
	int i, j;						//Loop variables
    int col, row, maxVal, value;	//Information of the picture taken from the file
    int kernel_size, window_size;	//Sizes to be used in filtering
    int isP5, type = -1;			//Variables related to the format of the file
    unsigned char charVal;			//Used to read P5 type file
    
    printf("Enter the name of the image you want to open: ");
    scanf("%s", filename);
    
    if((fp = fopen(filename, "rb")) == NULL) {
        printf("Error opening file!\n");
        fclose(fp);
        return -1;
    }
    
	//Skipping comment lines in file
    fgets(line, 2000, fp);
    while (line[0] == '#' || line[0] == '\n') {
        fgets (line, 2000, fp);
    };
    if (line[0] == 'P' && (line[1] == '2')) {
        isP5 = 0;
    }
    else if (line[0] == 'P' && (line[1] == '5')) {
        isP5 = 1;
    }
    else {
	   printf("The file is not in P2 or P5 format!\n");
	   fclose(fp);
	   return -1;
    }
    
	//Skipping comment lines in file
    fgets (line, 2000, fp);
        while (line[0] == '#' || line[0] == '\n') {
        fgets (line, 2000, fp);
    };
    
    //Reading the width, height and maximum gray level values in the image
    sscanf(line,"%d %d", &col, &row);
    fgets(line, 2000, fp);
    sscanf(line,"%d", &maxVal);
    
    int **image = create_matrix(row, col);
    int **filtered_image = create_matrix(row, col);
	
	//Depending on whether the picture is of P2 or P5 type, the reading process is performed
	if (isP5) {
		for(i = 0; i < row ;i++){
        	for(j = 0; j < col ;j++){
            	fread(&charVal, 1, 1, fp);
            	image[i][j] = (int) charVal;
        	}
    	}
	}
	else {
		for(i = 0; i < row; i++){
        	for(j = 0; j < col; j++){
            	fscanf(fp, "%d", &value);
            	image[i][j] = value;
        	}
    	}	
	}
    fclose(fp);

    printf("Filters:\n"
	"1) Mean filter\n"
	"2) Median filter\n\n"
	"Choose the filter you want to apply: ");
    scanf("%d",&type);
    
	while(type != 1 && type != 2){
    printf("\nYou have entered an incorrect number!\n"
	"Filters:\n"
	"1) Mean filter\n"
	"2) Median filter\n\n"
	"Choose the filter you want to apply: ");
	scanf("%d",&type);
	}
    
    if(type == 1){
    	printf("\nEnter the kernel size (e.g. enter 3 for 3x3): ");
    	scanf("%d", &kernel_size);
    	mean_filter(image, row, col, filtered_image, kernel_size);
	}
	else{
		printf("\nEnter the window size (e.g. enter 3 for 3x3): ");
    	scanf("%d", &window_size);
		median_filter(image, row, col, filtered_image, window_size);
	}
	
	printf("\nThe filter has been applied successfully.");
	
	printf("\nEnter the name (without extension) of the filtered image to be saved in PGM format: ");
    scanf("%s", savename);
	strcat(savename, ".pgm");
	
    create_PGM_file(filtered_image, col, row, maxVal, savename);

    return 0;
}
