//Ecrit par Xtreen
//debut 30/11/2012  derniere modification 22/12/2012 à 3h32
// Application de tampon pour images
//
#include <stdio.h>
#include <stdlib.h>
#include <SDL.h>




int main ( int argc, char** argv )
{
    if(SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        printf("Impossible d'Initialise SDL %s", SDL_GetError());
        exit(EXIT_FAILURE);
    }
SDL_Surface *ecran= NULL,  *image1= NULL,*cache=NULL, *image2=NULL, *image3 = NULL, *icone=NULL, *image[10]= {NULL}, *imageV[1500] ={NULL}, *imageC[1500]= {NULL};
icone = SDL_LoadBMP("pistolet.bmp");
SDL_WM_SetIcon(icone, NULL);



ecran = SDL_SetVideoMode(500, 375, 32, SDL_HWSURFACE|SDL_DOUBLEBUF);
if (!ecran)
{
    printf("Espace memoire insuffisant %s ", SDL_GetError());
    exit(EXIT_FAILURE);
}
//Chargement des images et Verification
image1 = SDL_LoadBMP("image\\radio.bmp");
if (!image1)
{
    printf("Image introuvable %s ", SDL_GetError());
    exit(EXIT_FAILURE);
}
image2 = SDL_LoadBMP("image\\ous2.bmp");
if (!image2)
{
    printf("Image introuvable %s ", SDL_GetError());
    exit(EXIT_FAILURE);
}
image3 = SDL_LoadBMP("image\\coeur.bmp");
if (!image3)
{
    printf("Image introuvable %s ", SDL_GetError());
    exit(EXIT_FAILURE);
}
// chargemeent des images Tampons
image[0] = SDL_LoadBMP("image\\benin.bmp");
image[1] = SDL_LoadBMP("image\\coeur2.bmp");
image[2] = SDL_LoadBMP("image\\aime.bmp");
image[3] = SDL_LoadBMP("image\\lion.bmp");
image[4] = SDL_LoadBMP("image\\jour.bmp");
image[5] = SDL_LoadBMP("image\\vole.bmp");
image[6] = SDL_LoadBMP("image\\aide.bmp");
//verification
long t=0;
for (t=0; t<7; t++)
{
    if (!image[t])
    {
        printf("Image introuvable !! %s ", SDL_GetError());
        exit(EXIT_FAILURE);
    }
}
//traitement des images
t=0;
for (t=0; t<4; t++)
SDL_SetColorKey(image[t], SDL_SRCCOLORKEY, SDL_MapRGB(image[t]->format, 0, 0, 0));

t=4;
for(t=4; t<6; t++ )
SDL_SetColorKey(image[t], SDL_SRCCOLORKEY, SDL_MapRGB(image[t]->format, 255, 255, 255));

SDL_SetColorKey(image3, SDL_SRCCOLORKEY, SDL_MapRGB(image3->format, 0, 0, 0));
// nom de la fenetre

SDL_WM_SetCaption("Xtreen Apps",NULL);
long i=0,g=0, h=0, j=0, waiting = 0;


SDL_Rect position, mouvement,  colageI[500];
position.x = 0;
position.y = 0;


mouvement.x = 0;
mouvement.y = 0;

// collage de l'image
SDL_SetColorKey(image1, SDL_SRCCOLORKEY, SDL_MapRGB(image1->format, 0, 0, 0));
SDL_SetAlpha(image1, SDL_SRCALPHA, 128);
SDL_BlitSurface(image2, NULL, ecran, &position);


SDL_Flip(ecran);

int continuer=1;
SDL_Event evenement, evenement2, evenement3;
SDL_EnableKeyRepeat(500, 10);

//Centrer le cruseur
     SDL_WarpMouse(ecran->w/2, ecran->h/2);
     i=0;
     j=0;
 while(continuer)
 {
     SDL_WaitEvent(&evenement);

         switch(evenement.type)
         {

             case SDL_QUIT:
             continuer = 0;
             break;

            case SDL_MOUSEMOTION:
           if (imageV[g] != NULL)
                 {
            mouvement.x = evenement.motion.x - imageV[g]->w/2;
            mouvement.y = evenement.motion.y - imageV[g]->h/2;
                 }
             case SDL_MOUSEBUTTONUP:
             switch(evenement.button.button)
             {
                 case SDL_BUTTON_LEFT:
                  if (imageV[g] !=NULL)
         {


                        imageC[j]= imageV[g];

                    colageI[j].x= evenement.button.x - imageC[j]->w/2;
                    colageI[j].y= evenement.button.y - imageC[j]->h/2;

                    j++;
         }

                 break;


             }
             case SDL_KEYDOWN:
             switch(evenement.key.keysym.sym)
             {
                 case SDLK_ESCAPE:
             continuer = 0;
             break;

                case SDLK_F1:
waiting=1;
// Sauvegarde de l'image avant l'affichage de l'image de l'aide
SDL_SaveBMP(ecran, "cache\\cache.bmp");
                while(waiting)
                {
                    SDL_WaitEvent(&evenement3);

                    SDL_BlitSurface(image[6], NULL, ecran, &position);
                    SDL_Flip(ecran);


                    switch(evenement3.type)
                    {
                        case SDL_QUIT:
                        waiting=0;
                        continuer=0;
                        break;

                        case SDL_KEYDOWN:
                        switch(evenement3.key.keysym.sym)
                        {
                            case SDLK_SPACE:
                            waiting = 0;
                            break;
                        }
                        break;
                    }
                }
                cache = SDL_LoadBMP("cache\\cache.bmp");
                SDL_BlitSurface(cache, NULL, ecran, &position);
                SDL_Flip(ecran);
                break;

             case SDLK_DELETE:
             SDL_BlitSurface(image2, NULL, ecran,&position);
             i=0;
             j=0;
             break;

             case SDLK_l:
             g++;
             imageV [g]= image[3];

             break;

             case SDLK_a:
             g++;
             imageV[g] = image[2];

             break;

             case SDLK_c:
             g++;
             imageV[g] = image[1];

             break;

             case SDLK_b:
             g++;
             imageV [g]= image[0];

             break;

             case SDLK_j:
             g++;
             imageV[g]= image[4];

             break;

             case SDLK_v:
             g++;
             imageV[g] = image[5];

             break;

             case SDLK_f:

    i=0;
    long vraie = 1;
    while(vraie)
    {
        SDL_WaitEvent(&evenement2);
        switch(evenement2.type)
        {
            case SDL_QUIT:
            vraie =0;
            continuer=0;
            break;
            case SDL_KEYDOWN:
            switch(evenement2.key.keysym.sym)
            {
                case SDLK_UP:
                i++;
                if (i>255)
                i=0;
                break;
                case SDLK_DOWN:
                i--;
                if (i<0)
                i=255;
                break;
                case SDLK_SPACE:
                i=0;
                break;
                case SDLK_ESCAPE:
                vraie = 0;
                break;

            }
            break;
        }

        SDL_FillRect(ecran, NULL, SDL_MapRGB(ecran->format, i, 255-i, 255));

        SDL_Flip(ecran);
    }

    break;

                case SDLK_LEFT:
                colageI[j].x--;
                break;

                case SDLK_RIGHT:
                 colageI[j].x++;

                break;

                case SDLK_UP:
                colageI[j].y--;
                break;

                case SDLK_DOWN:
                 colageI[j].y++;
                break;

                case SDLK_SPACE:
                if (SDL_SaveBMP(ecran, "image\\save\\Vigs.bmp"))
                printf("Sauvegarde echouee %s", SDL_GetError);

             }
         }
         if (imageV[g] !=NULL)
         {
        SDL_FillRect(ecran, NULL, SDL_MapRGB(ecran->format, i, 255-i, 255));
         h=0;
for (h=0; imageC[h] !=NULL; h++)
SDL_BlitSurface(imageC[h], NULL, ecran, &colageI[h]);

SDL_BlitSurface(imageV[g], NULL, ecran, &mouvement);


            SDL_Flip(ecran);
         }
    }
//liberation du Mémoire
    i=0;
for(i=0; i<7; i++)
SDL_FreeSurface(image[i]);

i=0;
for (i=0; imageC[i] !=NULL;i++ )
SDL_FreeSurface(imageC[i]);

i=0;
for (i=0; imageV[i] !=NULL;i++ )
SDL_FreeSurface(imageV[i]);

 SDL_FreeSurface(image1);
 SDL_FreeSurface(image3);
 SDL_FreeSurface(image2);
 SDL_Quit();

return 0;
}

