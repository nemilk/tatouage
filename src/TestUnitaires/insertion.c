#include "insertion.h"

void generationPar(int N,IplImage *img,IplImage *Logo){

  int i;
  srand(time(NULL));  

  key=alocavd(NWB); // On génère aléatoirement les clefs ( entre 0 et 0,25 )
  for(i=0;i<NWB;i++)
    key[i]=(((double)rand()/(RAND_MAX))/4);
  
  logoToBin (Logo); // On convertie le logo en binaires, ce sera le message à tatouer

  if ((KEY=fopen("Resultat/key.txt","w"))==NULL){
    printf("Erreur ouverture fichier.\n");
    exit (EXIT_FAILURE);
  }

  for (i=0;i<NWB;i++)
    fprintf(KEY,"%f\n",key[i]);
  fclose (KEY);

}

void selection(double **Mv,int N,IplImage *img){

  int i,j,z;
  int g=0;
  int mes_dec;
  int mes_i;
  double key_i;
  double aux1,aux2,w;           // Intermédiaires de calcul
  double Q;                     // Valeur test pour l'insertion
  double proj1,proj2;           // Projetés sur le masque

  float masque_t[4][4]= {
    {0,0.58,1.2,2.39},
    {0.58,1.12,1.49,2.3},
    {1.2,1.49,3.07,4.35},
    {2.39,2.3,4.35,7.25}
  };
  float Norme_masque=11.4492;
  
  double aux;
  double **blocT2;
  blocT2=alocamd(N,N);

  srand(time(NULL));  

  posx=alocavi(NWB);
  posy=alocavi(NWB);

  if ((POS=fopen("Resultat/position_bloc.txt","w"))==NULL){
    printf("Erreur ouverture fichier.\n");
    exit (EXIT_FAILURE);
  }

  if ((Mark=fopen("Resultat/Marque.txt","r"))==NULL){
    printf("Erreur ouverture fichier.\n");
    exit (EXIT_FAILURE);
  }

  if ((KEY=fopen("Resultat/key.txt","r"))==NULL){
    printf("Erreur ouverture fichier.\n");
    exit (EXIT_FAILURE);
  }

  for(z=0;z<NWB;z++){

    fscanf(Mark,"%d\n",&mes_i);
    fscanf(KEY,"%lf\n",&key_i);
    g=0;

    while (g!=6){

      proj1=0;
      proj2=0;
      posx[z] = N*(rand()%((img->width)/N));
      posy[z] = N*(rand()%((img->height)/N));  
      extractionBloc ( Mv, posx[z], posy[z],N, bloc);      // Extraction des blocs
      dct2dim (bloc, blocT,N,N);  // L'énergie se calcule à partie de la DCT des blocs
      for (i=0;i<N;i++)                // Copie de blocT dans blocT2
	for (j=0;j<N;j++)
	  blocT2[i][j]=blocT[i][j];
      
  
      for (i=0;i<N;i++)               // Transposée de blocT
	for(j=0;j<N;j++){

	  aux=blocT[i][j];
	  blocT[i][j]=blocT[j][i];
	  blocT[j][i]=aux;
	}
  
      for (i=0;i<N;i++)
	for(j=0;j<N;j++)               // Projection sur le masque
	  proj1+=(double)blocT[i][j]*masque_t[i][j]/Norme_masque;

      aux1= floor((proj1-pas*((float)mes_i/2+key_i))/pas+0.5);
      w = a*(aux1*pas-(proj1-pas*((float)mes_i/2+key_i)));  

      /*insertion*/

      for (i=0;i<N;i++)
	for(j=0;j<N;j++){
	  //blocT[i][j]=0;
	  blocT[i][j]=blocT2[i][j]+w*masque_t[i][j]/Norme_masque;
	}

      /*detection immédiate*/

      for (i=0;i<N;i++)
	for(j=0;j<N;j++)
	  proj2+=(double)blocT[i][j]*masque_t[i][j]/Norme_masque;

      aux2=floor((proj2-pas*key_i)/(pas)+0.5);
      Q = aux2*pas-(proj2-pas*key_i);

      if(mes_i==0 && fabs(Q)<(1-a)*pas/10){
	fprintf(POS,"%d %d\n",posx[z],posy[z]);
	g=6;
      }
  
      if(mes_i==1 && fabs(Q)>(1-a)*pas/2){
	fprintf(POS,"%d %d\n",posx[z],posy[z]);
	g=6;
      }
    }
  } 

  dalocd(blocT2,N);
  fclose(POS);
  fclose(Mark);
  fclose(KEY);
}

void insertionMarque (double **blocT,int N,double key_i,int mes_i){

	int i,j;
	int mes_dec;
	double aux1,aux2,w;           // Intermédiaires de calcul
	double Q;                     // Valeur test pour l'insertion
	double proj1,proj2;           // Projetés sur le masque
	proj1=0;
	proj2=0;

	float masque_t[4][4]= {
		{0,0.58,1.2,2.39},
		{0.58,1.12,1.49,2.3},
		{1.2,1.49,3.07,4.35},
		{2.39,2.3,4.35,7.25}
	};
	float Norme_masque=11.4492;

	double aux;
	double **blocT2;
	blocT2=alocamd(N,N);

	for (i=0;i<N;i++)                // Copie de blocT dans blocT2
		for (j=0;j<N;j++)
			blocT2[i][j]=blocT[i][j];


	for (i=0;i<N;i++)               // Transposée de blocT
		for(j=0;j<N;j++){

			aux=blocT[i][j];
			blocT[i][j]=blocT[j][i];
			blocT[j][i]=aux;
		}

	for (i=0;i<N;i++)
		for(j=0;j<N;j++)               // Projection sur le masque
			proj1+=(double)blocT[i][j]*masque_t[i][j]/Norme_masque;

	aux1= floor((proj1-pas*((float)mes_i/2+key_i))/pas+0.5);
	w = a*(aux1*pas-(proj1-pas*((float)mes_i/2+key_i)));  

	/*insertion*/

	for (i=0;i<N;i++)
		for(j=0;j<N;j++){
			//blocT[i][j]=0;
			blocT[i][j]=blocT2[i][j]+w*masque_t[i][j]/Norme_masque;
		}

	/*detection immédiate*/

	for (i=0;i<N;i++)
		for(j=0;j<N;j++)
			proj2+=(double)blocT[i][j]*masque_t[i][j]/Norme_masque;

	aux2=floor((proj2-pas*key_i)/(pas)+0.5);
	Q = aux2*pas-(proj2-pas*key_i);

	if (fabs(Q)<(1-a)*pas/2)
		mes_dec=0;
	else
		mes_dec=1;

	if(mes_dec!=mes_i){
		printf("Erreur lors de l'insertion.\n");
		exit(EXIT_FAILURE);
	}
	dalocd(blocT2,N);
}

void Insertion (IplImage *img,int N,int *p){

	int i;
	double **Mv;
	Mv=alocamd(img->width, img->height); 

	IplImage *imgHSV;         // Image initiale en HSV
	IplImage *imgHSVTat;         // Image tatouée en HSV
	IplImage *imgTat;           // Image tatouée en RGB
	imgHSV=cvCreateImage(cvGetSize(img),img->depth,img->nChannels) ; 
	imgHSVTat=cvCreateImage(cvGetSize(img),img->depth,img->nChannels) ;
	imgTat=cvCreateImage(cvGetSize(img),img->depth,img->nChannels);


	cvCvtColor(img,imgHSV,CV_BGR2HSV); // On met dans imgHSV l'image sources e
	extractionCanalV(Mv,imgHSV);
	generationPar(N,img,Logo);
	selection(Mv,N,img);

	if ((Mark=fopen("Resultat/Marque.txt","r"))==NULL){
		printf("Erreur ouverture fichier.\n");
		exit (EXIT_FAILURE);
	}

	if ((POS=fopen("Resultat/position_bloc.txt","r"))==NULL){
		printf("Erreur ouverture fichier.\n");
		exit (EXIT_FAILURE);
	}

	for (i=0;i<NWB;i++) {
		fscanf(Mark,"%d\n",&mes);
		extractionBloc ( Mv, posx[i], posy[i],N, bloc);      // Extraction des blocs
		dct2dim (bloc, blocT,N,N);              // DCT sur le bloc sélectionné
		insertionMarque(blocT,N,key[i],mes);      // Insertion du message et validation de l'insertion
		dct2dim_inv (blocT, bloc,N,N);          // DCT inverse sur le bloc tatoué
		insertionBloc (bloc, posx[i],posy[i],N,Mv); // Insertion du bloc tatoué dans la matrice de l'image
	}

	fclose(POS);
	fclose(Mark);

	matToImg(Mv,imgHSV,imgHSVTat); // Création de l'image tatouée à partir de Mv et de l'image source
	cvCvtColor(imgHSVTat,imgTat,CV_HSV2BGR); // On convertit l'image HSV tatouée en image RGB
	cvSaveImage ("Resultat/imageTatouee.jpg", imgTat, p);
	cvSaveImage ("Resultat/imageSource.jpg", img, p);
	cvSaveImage ("Resultat/logoSource.jpg", Logo, p);


	calculPSNR(img);

	cvReleaseImage(&imgTat);
	cvReleaseImage(&imgHSV);
	cvReleaseImage(&imgHSVTat);

}

