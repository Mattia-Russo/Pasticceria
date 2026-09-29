//in update segna quanto ce n'è in magazzino, in refill aumento la quantità totale, quando uso decremento, quando controllo vedo se ne ho abbastanza
//rimuovi ricetta, controllo prima se c'è


#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX_LEN_NAME 255
#define HASH_TABLE_SLOTS 30000

typedef struct ingred_mag{
    char* name;
    int quantity;
    int expiration;
    unsigned short int hash_val;
    struct ingred_mag* next;
    struct ingred_mag* prev;
}t_ingred_mag;

typedef struct ingred_rec{
    char* name;
    int quantity;
    unsigned short int hash_val;
    struct ingred_rec* next;
    struct ingred_rec* prev;
}t_ingred_rec;

typedef struct recipe{
    char* name;
    struct ingred_rec* head;
    unsigned short int hash_val;
    struct recipe* next;
    struct recipe* prev;
}t_recipe;

typedef struct list{
    struct ingred_mag* ingred;
    int quant;
    struct list* next;
}t_list;

typedef struct order{
    struct recipe* recipe;
    int weight;
    int quantity;
    int time_unit;
    int miss_time;
    struct update_im* missing_ptr;
    struct list* list;
    struct order* next;
    struct order* prev;
}t_order;

typedef struct name_check{
    struct order* order;
    int quantity;
    struct name_check* next;
    struct name_check* prev;
}t_name_check;

typedef struct update_im{
    char* name;
    int time_unit;
    struct update_im* next;
    struct update_im* prev;
}t_update_im;

unsigned short int h(char* name);

void insert_order(t_order** head, t_order** tail, t_order* node);
void move_order(t_order** head, t_order** tail, t_order* node);
void remove_order(t_order** head, t_order** tail, t_order* node);
void insert_van(t_order** van, t_order* load);

void remove_ingr(t_ingred_mag **magazine, t_ingred_mag* ptr_im);
void decrease_ingr(t_ingred_mag** magazine, t_order* ord);
short int check_ingred(t_ingred_mag** magazine, t_order* ord, int time, t_update_im** updated);

void save_ingr_rec(char  buffer[MAX_LEN_NAME+1], t_ingred_rec** head);
void insert_new_rec(t_recipe **cookbook, char  buffer[MAX_LEN_NAME+1], short int hash_val);
void delete_recipe(t_recipe **cookbook, t_recipe* rec);
t_recipe* check_recipe(t_recipe **cookbook, char  buffer[MAX_LEN_NAME+1]);

void delete_expired(t_ingred_mag** magazine, int time);
void insert_update_im(t_update_im** updated, int time, t_ingred_mag* ptr_im, unsigned short int hash_val);
void refill(t_ingred_mag** magazine, char  buffer[MAX_LEN_NAME+1], t_update_im** updated, int time);

void insert_name_check(t_name_check** already_checked, t_order* ptr_o);
void check_wo(t_ingred_mag** magazine, t_order** wo_head, t_order** wo_tail, t_order** ro_head, t_order** ro_tail, int time, t_name_check** already_checked, t_update_im** updated);
void check_ro(t_ingred_mag** magazine, t_recipe* ptr_r, t_order** wo_head, t_order** wo_tail, t_order** ro_head, t_order** ro_tail, t_name_check** already_checked, t_order* new_o, int quantity_req, int time, t_update_im** updated);


int main(void){             //RIDURRE DIMENSIONI VARIABILI
    //lettura informazioni camioncino
    unsigned int capacity=0, period=0;
    if(!(scanf("%d %d\n", &period, &capacity))){
        return 0;
    }

    //inizializzazione strutture dati necessarie
    t_recipe* cookbook[HASH_TABLE_SLOTS];
    t_ingred_mag* magazine[HASH_TABLE_SLOTS];
    t_order* ro_head=NULL;  //ready orders head (List)
    t_order* ro_tail=NULL;
    t_order* wo_head=NULL; //waiting orders head (List)
    t_order* wo_tail=NULL;
    t_name_check* already_checked[HASH_TABLE_SLOTS];
    t_update_im* updated[HASH_TABLE_SLOTS];

    unsigned int time=0;
    short int flag;
    
    //inizializzazione array contenenti buffer, comando, ricetta
    char ch='\0', buffer[MAX_LEN_NAME+1];
    unsigned short int i=0;
    for(i=0;i<MAX_LEN_NAME; i++){
        buffer[i]='\0';
    }
    for(i=0;i<HASH_TABLE_SLOTS;i++){
        cookbook[i]=NULL;
        magazine[i]=NULL;
        already_checked[i]=NULL;
        updated[i]=NULL;
    }

    while((ch=getchar())!=EOF || time%period==0){
        
        // printf("%d ", time);
        //GESTIONE ARRIVO CAMIONCINO
        if(time!=0 && time%period==0){
            if(ro_head==NULL){
                printf("camioncino vuoto\n");
            }
            else{
                int tmp_capacity=capacity;
                flag=1;
                t_order* load=NULL, *succ=NULL, *van=NULL;;
                load=ro_head;
                while(load!=NULL && tmp_capacity>=load->weight){
                    succ=load->next;
                    tmp_capacity-=load->weight;

                    // rimozione da ro list
                    remove_order(&ro_head, &ro_tail, load);
                    
                    //inserimento in van list

                    insert_van(&van, load);
                    
                    load=succ;
                }

                for(load=van; load!=NULL;){
                    printf("%d %s %d\n", load->time_unit, load->recipe->name, load->quantity);
                    t_order* ptr=load;
                    load=load->next;
                    free(ptr);
                }
            }
        }
        
        //lettura comando
        if(ch=='\n'){
            ch=getchar();
        }
        if(ch==EOF){
            break;
        }
        buffer[0]=ch;
        for(i=1;(ch=getchar())!=' ' && i<MAX_LEN_NAME; i++){
            buffer[i]=ch;
        }
        buffer[i]='\0';

        if(strcmp(buffer, "aggiungi_ricetta")==0){

            for(i=0;(ch=getchar())!=' ' && i<MAX_LEN_NAME; i++){
                buffer[i]=ch;
            }
            //scrivo la fine della stringa
            buffer[i]='\0';
            
            unsigned short int hash_val=h(buffer);
            t_recipe* ptr_hr=NULL, *ptr_tr=NULL;
            flag=0;

            if(cookbook[hash_val]!=NULL && cookbook[hash_val]->next==cookbook[hash_val] && strcmp(cookbook[hash_val]->name, buffer)!=0){
                ptr_hr=NULL;
            }
            else{
                ptr_hr=cookbook[hash_val];
            }
            if(ptr_hr!=NULL){
                ptr_hr=cookbook[hash_val];
                ptr_tr=cookbook[hash_val]->prev;
                while(ptr_hr!=ptr_tr && ptr_hr!=ptr_tr->prev){
                    if(strcmp(ptr_hr->name, buffer)==0){
                        flag=1;
                        break;
                    }
                    if(strcmp(ptr_tr->name, buffer)==0){
                        ptr_hr=ptr_tr;
                        flag=1;
                        break;
                    }
                    ptr_hr=ptr_hr->next;
                    ptr_tr=ptr_tr->prev;
                }
                if(strcmp(ptr_hr->name, buffer)==0){
                    flag=1;
                }
                else if(strcmp(ptr_tr->name, buffer)==0){
                    ptr_hr=ptr_tr;
                    flag=1;
                }
            }

            if(!flag){
                insert_new_rec(cookbook, buffer, hash_val);
                printf("aggiunta\n");    
            }
            else{
                printf("ignorato\n");
                while((ch=getchar())!='\n' && ch!=EOF);
            }
        }

        else if(strcmp(buffer, "rimuovi_ricetta")==0){

            //copio il nome della ricetta da rimuovere
            for(i=0;((ch=getchar())!=' ' && ch!='\r' && ch!='\n' && ch!=EOF)&& i<MAX_LEN_NAME; i++){
                buffer[i]=ch;
            }
            buffer[i]='\0';
            
            t_recipe* ptr_r=check_recipe(cookbook, buffer);

            if(ptr_r){

                flag=1;
                t_order* ptr_h=NULL, *ptr_t=NULL;

                //controllo se ordine in attesa di produzione
                for(ptr_h=wo_head, ptr_t=wo_tail; ptr_h!=NULL && flag; ptr_h=ptr_h->next, ptr_t=ptr_t->prev){
                    if(strcmp(buffer, ptr_h->recipe->name)==0 || strcmp(buffer, ptr_t->recipe->name)==0){
                        printf("ordini in sospeso\n");
                        flag=0;
                    }
                    if(ptr_h==ptr_t || ptr_h==ptr_t->next){
                        break;
                    }
                }

                //controllo se ordine in attesa di spedizione
                for(ptr_h=ro_head, ptr_t=ro_tail; ptr_h!=NULL && flag; ptr_h=ptr_h->next, ptr_t=ptr_t->prev){
                    if(strcmp(buffer, ptr_h->recipe->name)==0 || strcmp(buffer, ptr_t->recipe->name)==0){
                        printf("ordini in sospeso\n");
                        flag=0;
                    }
                    if(ptr_h==ptr_t || ptr_h==ptr_t->next){
                        break;
                    }
                }

                if(flag){
                    delete_recipe(cookbook, ptr_r);
                    printf("rimossa\n");
                }
            }
            else{
                printf("non presente\n");
            }
        }

        else if(strcmp(buffer, "rifornimento")==0){

            //elimino ingredienti scaduti nel magazzino
            delete_expired(magazine, time);
            
            //inizio rifornimento

            refill(magazine, buffer, updated, time);
            
            printf("rifornito\n");

            for(i=0; i<HASH_TABLE_SLOTS; i++){
                if(already_checked[i]!=NULL){
                    if(already_checked[i]->next==already_checked[i]){
                        free(already_checked[i]);
                    }
                    else{
                        t_name_check* ptr=already_checked[i], *ptr2=NULL;
                        ptr->prev->next=NULL;
                        
                        while(ptr!=NULL){
                            ptr2=ptr;
                            ptr=ptr->next;
                            free(ptr2);
                        }
                    }
                    already_checked[i]=NULL;
                }
            }

            //controllo fattibilità ordini attesa

            check_wo(magazine, &wo_head, &wo_tail, &ro_head, &ro_tail, time, already_checked, updated);
            
        }

        else if(strcmp(buffer, "ordine")==0){
            
            for(i=0;(ch=getchar())!=' ' && i<MAX_LEN_NAME; i++){
                buffer[i]=ch;
            }
            buffer[i]='\0';

            t_recipe* ptr_hr, *ptr_tr;
            unsigned short int hash_val=h(buffer);
            flag=0;

            //mi posiziono con ptr_r sulla ricetta richiesta
            if(cookbook[hash_val]!=NULL && cookbook[hash_val]->next==cookbook[hash_val] && strcmp(cookbook[hash_val]->name, buffer)!=0){
                ptr_hr=NULL;
            }
            else{
                ptr_hr=cookbook[hash_val];
            }
            if(ptr_hr!=NULL){
                ptr_hr=cookbook[hash_val];
                ptr_tr=cookbook[hash_val]->prev;
                while(ptr_hr!=ptr_tr && ptr_hr!=ptr_tr->prev){
                    if(strcmp(ptr_hr->name, buffer)==0){
                        flag=1;
                        break;
                    }
                    if(strcmp(ptr_tr->name, buffer)==0){
                        ptr_hr=ptr_tr;
                        flag=1;
                        break;
                    }
                    ptr_hr=ptr_hr->next;
                    ptr_tr=ptr_tr->prev;
                }
                if(strcmp(ptr_hr->name, buffer)==0){
                    flag=1;
                }
                else if(strcmp(ptr_tr->name, buffer)==0){
                    ptr_hr=ptr_tr;
                    flag=1;
                }
            }

            if(flag){
                //leggo quanti ordini dello stesso dolce sono arrivati
                for(i=0;(ch=getchar())!='\n' && ch!='\r' && ch!=EOF;i++){
                    buffer[i]=ch;
                }
                buffer[i]='\0';
                int quantity_req = atoi(buffer); //quantity required from the order

                //calcolo il peso dell'ordine (somma dei pesi degli ingredienti * quanti elementi sono richiesti)
                unsigned int dim=0;
                t_ingred_rec* ptr_ir=ptr_hr->head, *ptr_t=ptr_ir;
                do{
                    dim+=ptr_ir->quantity;
                    ptr_ir=ptr_ir->next;
                }while(ptr_ir!=ptr_t);
                
                t_order* new_o;
                new_o = malloc(sizeof(t_order));
                new_o->prev=NULL;
                new_o->next=NULL;
                new_o->recipe=ptr_hr;
                new_o->list=NULL;
                new_o->missing_ptr=NULL;
                new_o->miss_time=0;
                new_o->weight=quantity_req*dim;
                new_o->time_unit=time;
                new_o->quantity=quantity_req;
                
                //controllo che ho tutti gli ingredienti

                check_ro(magazine, ptr_hr, &wo_head, &wo_tail, &ro_head, &ro_tail, already_checked, new_o, quantity_req, time, updated);
                
                
                printf("accettato\n");
            }
            else{
                printf("rifiutato\n");
                while((ch=getchar())!='\n');
            }
        }
        
        time++;

    }

    //free di tutta la memoria utilizzata
    for(i=0;i<HASH_TABLE_SLOTS;i++){

        //free magazzino

        if(magazine[i]!=NULL){
            if(magazine[i]->next==magazine[i]){
                free(magazine[i]->name);
                free(magazine[i]);
            }
            else{
                t_ingred_mag* ptr=magazine[i], *ptr2=NULL;
                ptr->prev->next=NULL;
                while(ptr!=NULL){
                    ptr2=ptr;
                    ptr=ptr->next;
                    free(ptr2->name);
                    free(ptr2);
                }
            }
        }

        //free ricettario

        if(cookbook[i]!=NULL){
            if(cookbook[i]->next==cookbook[i]){
                t_ingred_rec* ptr=cookbook[i]->head, *ptr2;
                ptr->prev->next=NULL;
                while(ptr!=NULL){
                    ptr2=ptr;
                    ptr=ptr->next;
                    free(ptr2->name);
                    free(ptr2);
                }
                free(cookbook[i]->name);
                free(cookbook[i]);
            }
            else{
                t_recipe* ptr=cookbook[i], *ptr2=NULL;
                ptr->prev->next=NULL;
                while(ptr!=NULL){
                    ptr2=ptr;
                    ptr=ptr->next;
                    t_ingred_rec* ptr_i=ptr2->head, *ptr_i2;
                    ptr_i->prev->next=NULL;
                    while(ptr_i!=NULL){
                        ptr_i2=ptr_i;
                        ptr_i=ptr_i->next;
                        free(ptr_i2->name);
                        free(ptr_i2);
                    }
                    free(ptr2->name);
                    free(ptr2);
                }
            }
        }

        //free already_checked
        if(already_checked[i]!=NULL){
            if(already_checked[i]->next==already_checked[i]){
                free(already_checked[i]);
            }
            else{
                t_name_check* ptr=already_checked[i], *ptr2=NULL;
                ptr->prev->next=NULL;
                
                while(ptr!=NULL){
                    ptr2=ptr;
                    ptr=ptr->next;
                    free(ptr2);
                }
            }
        }

        // updated
        if(updated[i]!=NULL){
            if(updated[i]->next==updated[i]){
                free(updated[i]);
            }
            else{
                t_update_im* ptr=updated[i], *ptr2=NULL;
                ptr->prev->next=NULL;
                
                while(ptr!=NULL){
                    ptr2=ptr;
                    ptr=ptr->next;
                    free(ptr2);
                }
            }
        }
    }

    //free order wait
    t_order* ptr1=NULL, *ptr2=NULL;
    ptr1=wo_head;
    while(ptr1!=NULL){
        ptr2=ptr1;
        ptr1=ptr1->next;
        free(ptr2);
    }

    //free order ready
    ptr1=ro_head;
    while(ptr1!=NULL){
        ptr2=ptr1;
        ptr1=ptr1->next;
        free(ptr2);
    }
}

unsigned short int h(char *name) {
    unsigned long int sum=0;
    unsigned short int val=0;
    while((val=*name)){
        sum=sum*31+val;
        name++;
    }
    return sum%HASH_TABLE_SLOTS;
}

void insert_order(t_order** head, t_order** tail, t_order* node){
    if(*head==NULL){
        *head=node;
        *tail=node;
        node->next=NULL;
        node->prev=NULL;
    }
    else{
        (*tail)->next=node;
        node->prev=*tail;
        node->next=NULL;
        *tail=node;
    }
}

void move_order(t_order** head, t_order** tail, t_order* node){
    
    if(*head==NULL){
        *head=node;
        *tail=node;
        node->next=NULL;
        node->prev=NULL;
    }
    else{
        if(*head==*tail){
            if(node->time_unit>(*head)->time_unit){
                (*head)->next=node;
                node->prev=*head;
                *tail=node;
                node->next=NULL;
            }
            else{
                (*head)->prev=node;
                node->next=*head;
                *head=node;
                node->prev=NULL;
            }
        }
        else{
            t_order* ptr1=*head, *ptr2=*tail;
            while(node->time_unit>ptr1->time_unit){
                if(node->time_unit>ptr2->prev->time_unit){
                    ptr1=ptr2;
                    break;
                }
                ptr2=ptr2->prev;
                ptr1=ptr1->next;
            }
            if(ptr1!=NULL){
                if(ptr1->next==NULL){
                    if(ptr1->time_unit<node->time_unit){
                        ptr1->next=node;
                        node->prev=ptr1;
                        node->next=NULL;
                        *tail=node;
                    }
                    else{
                        node->next=ptr1;
                        node->prev=ptr1->prev;
                        ptr1->prev->next=node;
                        ptr1->prev=node;
                    }
                    
                }
                else if(ptr1->prev==NULL){
                    ptr1->prev=node;
                    node->next=ptr1;
                    node->prev=NULL;
                    *head=node;
                }
                else{
                    node->next=ptr1;
                    node->prev=ptr1->prev;
                    ptr1->prev->next=node;
                    ptr1->prev=node;
                }
            }
        }        
    }
}

void remove_order(t_order** head, t_order** tail, t_order* node){

    if(node){
        if(node->next==NULL && node->prev==NULL){
            *head=NULL;
            *tail=NULL;
        }
        else if(node==*tail){
            *tail=(*tail)->prev;
            (*tail)->next=NULL;
        }
        else if(*head==node){
            *head=(*head)->next;
            (*head)->prev=NULL;
        }
        else{
            node->next->prev=node->prev;
            node->prev->next=node->next;
        }
    }
}

void save_ingr_rec(char  buffer[MAX_LEN_NAME+1], t_ingred_rec** head){
    
    char ch='\0';
    unsigned short int i=0;
    
    while(ch!='\n' && ch!=EOF && ch!='\r'){
        for(i=0;(ch=getchar())!=' ' && i<MAX_LEN_NAME; i++){
            buffer[i]=ch;
        }
        buffer[i]='\0';

        t_ingred_rec* new_ir;
        new_ir = malloc(sizeof(t_ingred_rec));
        new_ir->name = malloc(strlen(buffer)+1);
        strcpy(new_ir->name,buffer);
        new_ir->hash_val=h(new_ir->name);

        if(*head==NULL){
            new_ir->prev=new_ir;
            new_ir->next=new_ir;
            *head=new_ir;
        }
        else{
            new_ir->next=(*head)->next;
            new_ir->next->prev=new_ir;
            (*head)->next=new_ir;
            new_ir->prev=*head;
        }

        for(i=0; (ch=getchar())!=' ' && ch!='\r' && ch!=EOF && ch!='\n'; i++){
            buffer[i]=ch;
        }
        buffer[i]='\0';
        new_ir->quantity=atoi(buffer);
    }
}

void insert_new_rec(t_recipe **cookbook, char  buffer[MAX_LEN_NAME+1], short int hash_val){
    
    t_recipe* new_r;
    new_r = malloc(sizeof(t_recipe));
    new_r->head=NULL;
    new_r->hash_val=hash_val;
    new_r->name= malloc(strlen(buffer)+1);
    strcpy(new_r->name,buffer);
    
    if(cookbook[hash_val]==NULL){
        cookbook[hash_val]=new_r;
        new_r->next=new_r;
        new_r->prev=new_r;
    }            
    else{
        new_r->prev=cookbook[hash_val]->prev;
        new_r->next=cookbook[hash_val];
        cookbook[hash_val]->prev->next=new_r;
        cookbook[hash_val]->prev=new_r;
    }

    save_ingr_rec(buffer, &(new_r->head));
}

void insert_van(t_order** van, t_order* load){
    if(*van==NULL){
        *van=load;
        load->next=NULL;
        load->prev=NULL;
    }
    else{
        t_order* ptr1=NULL, *ptr2=NULL;
        for(ptr1=*van; ptr1!=NULL && load->weight<ptr1->weight; ptr2=ptr1, ptr1=ptr1->next);
        if(ptr1==NULL){
            ptr2->next=load;
            load->prev=ptr2;
            load->next=NULL;
        }
        else{
            if(load->weight==ptr1->weight){
                for(;ptr1!=NULL && load->weight== ptr1->weight && load->time_unit>ptr1->time_unit; ptr2=ptr1, ptr1=ptr1->next);
            }
            if(ptr1==NULL){
                ptr2->next=load;
                load->prev=ptr2;
                load->next=NULL;
            }
            else if(ptr1->prev==NULL){
                (*van)->prev=load;
                load->next=*van;
                load->prev=NULL;
                *van=load;
            }
            else{
                ptr2->next=load;
                ptr1->prev=load;
                load->next=ptr1;
                load->prev=ptr2;
            }
        }
    }
}

void delete_recipe(t_recipe **cookbook, t_recipe* rec){
    
    if(rec->next==rec){
        cookbook[rec->hash_val]=NULL;
    }
    else{
        rec->prev->next=rec->next;
        rec->next->prev=rec->prev;   
    }
    t_ingred_rec *ptr_ir=rec->head, *ptr=rec->head;
    while(ptr_ir!=ptr && ptr!=ptr_ir->next){
        ptr_ir=ptr_ir->next;
        ptr=ptr->prev;
        free(ptr->next->name);
        free(ptr->next);
        free(ptr_ir->prev->name);
        free(ptr_ir->prev);
    }
    if(ptr_ir!=ptr){
        free(ptr->name);
        free(ptr);
    }
    free(ptr_ir->name);
    free(ptr_ir);
    free(rec->name);
    free(rec);
}

t_recipe* check_recipe(t_recipe **cookbook, char  buffer[MAX_LEN_NAME+1]){
    
    unsigned short int hash_val=h(buffer);
    
    
    if(cookbook[hash_val]==NULL){
        return NULL;
    }
    else if(cookbook[hash_val]->next==cookbook[hash_val] && strcmp(cookbook[hash_val]->name, buffer)!=0){
        return NULL;
    }
    else{
        t_recipe* ptr_h=cookbook[hash_val], *ptr_t=cookbook[hash_val]->prev;

        while(ptr_h!=ptr_t && ptr_h!=ptr_t->prev){
            if(strcmp(ptr_h->name, buffer)==0){
                return ptr_h;
            }
            if(strcmp(ptr_t->name, buffer)==0){
                return ptr_t;
            }
            ptr_h=ptr_h->next;
            ptr_t=ptr_t->prev;
        }
        if(strcmp(ptr_h->name, buffer)==0){
            return ptr_h;
        }
        else if(strcmp(ptr_t->name, buffer)==0){
            return ptr_t;
        }
    }
    return NULL;
}

void delete_expired(t_ingred_mag** magazine, int time){
    
    t_ingred_mag *ptr2=NULL, *ptr=NULL;;
    unsigned short int i;

    for(i=0; i<HASH_TABLE_SLOTS; i++){
        if(magazine[i]!=NULL && magazine[i]->expiration<=time){
            ptr=magazine[i];
            do{
                ptr2=ptr;
                if(ptr!=ptr->next){
                    ptr=ptr->next;
                }
                else{
                    ptr=NULL;
                }
                if(ptr2->expiration<=time){
                    if(ptr2==magazine[i]){
                        magazine[i]=ptr;
                    }
                    remove_ingr(magazine, ptr2);
                }
            }while(ptr!=NULL && ptr->expiration<=time);
        }
    }
}

void insert_update_im(t_update_im** updated, int time, t_ingred_mag* ptr_im, unsigned short int hash_val){
    
    if(updated[hash_val]==NULL){
        t_update_im* new=malloc(sizeof(t_update_im));
        new->name=malloc(strlen(ptr_im->name)+1);
        strcpy(new->name, ptr_im->name);
        new->time_unit=time;
        new->next=new;
        new->prev=new;

        updated[hash_val]=new;
    }
    else if(updated[hash_val]->next==updated[hash_val]){
        short int val=strcmp(updated[hash_val]->name, ptr_im->name);
        if(val==0){
            updated[hash_val]->time_unit=time;
        }
        else{
            t_update_im* new=malloc(sizeof(t_update_im));
            new->name=malloc(strlen(ptr_im->name)+1);
            strcpy(new->name, ptr_im->name);
            new->time_unit=time;
            new->next=updated[hash_val];
            new->prev=updated[hash_val];
            updated[hash_val]->next=new;
            updated[hash_val]->prev=new;
            if(val<0){
                updated[hash_val]=new;
            }
        }
    }
    else{
        t_update_im* ptr_hu=updated[hash_val], *ptr_tu=updated[hash_val]->prev;
        short int val_h=-1, val_t=1;
        do{
            val_h=strcmp(ptr_hu->name, ptr_im->name);
            if(val_h==0){
                ptr_hu->time_unit=time;
                break;
            }
            val_t=strcmp(ptr_tu->name, ptr_im->name);
            if(val_t==0){
                ptr_tu->time_unit=time;
                break;
            }
            ptr_hu=ptr_hu->next;
            ptr_tu=ptr_tu->prev;
        }while(val_h<0 && val_t>0 && ptr_hu!=ptr_tu->next);

        if((val_h>0 || ptr_hu==ptr_tu->next) && val_h!=0 && val_t!=0){
            t_update_im* new=malloc(sizeof(t_update_im));
            new->name=malloc(strlen(ptr_im->name)+1);
            strcpy(new->name, ptr_im->name);
            new->time_unit=time;
            new->next=ptr_hu;
            new->prev=ptr_hu->prev;
            ptr_hu->prev->next=new;
            ptr_hu->prev=new;
            if(ptr_hu==updated[hash_val]){
                updated[hash_val]=new;
            }
        }
        else if(val_t<0 && val_h!=0){
            t_update_im* new=malloc(sizeof(t_update_im));
            new->name=malloc(strlen(ptr_im->name)+1);
            strcpy(new->name, ptr_im->name);
            new->time_unit=time;
            new->prev=ptr_tu;
            new->next=ptr_tu->next;
            ptr_tu->next->prev=new;
            ptr_tu->next=new;
        }
    }
}

void refill(t_ingred_mag** magazine, char  buffer[MAX_LEN_NAME+1], t_update_im** updated, int time){
    
    char ch='\0';
    unsigned short int i=0;

    while(ch!='\n' && ch!='\r' && ch!=EOF){

        for(i=0;(ch=getchar())!=' ' && i<MAX_LEN_NAME; i++){
            buffer[i]=ch;
        }
        buffer[i]='\0';

        char* name=malloc(strlen(buffer)+1);
        strcpy(name, buffer);

        for(i=0; (ch=getchar())!=' '; i++){
            buffer[i]=ch;
        }
        buffer[i]='\0';
        int quantity=atoi(buffer);

        for(i=0; (ch=getchar())!=' ' && ch!='\r' && ch!= '\n' && ch!=EOF; i++){
            buffer[i]=ch;
        }
        buffer[i]='\0';
        int expiration=atoi(buffer);

        unsigned short int hash_val=h(name);

        if(magazine[hash_val]==NULL){
            t_ingred_mag* new_im;
            new_im = malloc(sizeof(t_ingred_mag));
            new_im->name=malloc(strlen(name)+1);
            strcpy(new_im->name,name);
            free(name);
            new_im->quantity=quantity;
            new_im->expiration=expiration;
            new_im->hash_val=hash_val;

            magazine[hash_val]=new_im;
            new_im->next=new_im;
            new_im->prev=new_im;

            insert_update_im(updated, time, new_im, hash_val);
        }            
        else{
            t_ingred_mag* ptr_h=magazine[hash_val], *ptr_t=magazine[hash_val];
            short int flag=1;

            while(ptr_h->expiration<expiration){
                if(ptr_t->prev->expiration<expiration){
                    flag=2;
                    break;
                }
                ptr_h=ptr_h->next;
                ptr_t=ptr_t->prev;
            }
            if(flag==1){
                while(ptr_h->expiration==expiration){
                    if(strcmp(name, ptr_h->name)==0){
                        flag=2;
                        break;
                    }
                    if(ptr_h==ptr_t || ptr_h==ptr_t->next){
                        break;
                    }
                    ptr_h=ptr_h->next;
                }
                if(flag==2){
                    ptr_h->quantity+=quantity;
                    insert_update_im(updated, time, ptr_h, hash_val);
                }
                else{
                    t_ingred_mag* new_im;
                    new_im = malloc(sizeof(t_ingred_mag));
                    new_im->name=malloc(strlen(name)+1);
                    strcpy(new_im->name,name);
                    free(name);
                    new_im->quantity=quantity;
                    new_im->expiration=expiration;
                    new_im->hash_val=hash_val;
                    
                    new_im->prev=ptr_h->prev;
                    new_im->next=ptr_h;
                    ptr_h->prev->next=new_im;
                    ptr_h->prev=new_im;

                    insert_update_im(updated, time, new_im, hash_val);

                }
            }
            else if(flag==2){
                while(ptr_t->expiration==expiration){
                    if(strcmp(name, ptr_t->name)==0){
                        flag=1;
                        break;
                    }
                    ptr_t=ptr_t->prev;
                }
                if(flag==1){
                    ptr_t->quantity+=quantity;
                    insert_update_im(updated, time, ptr_t, hash_val);
                }
                else{
                    t_ingred_mag* new_im;
                    new_im = malloc(sizeof(t_ingred_mag));
                    new_im->name=malloc(strlen(name)+1);
                    strcpy(new_im->name,name);
                    free(name);
                    new_im->quantity=quantity;
                    new_im->expiration=expiration;
                    new_im->hash_val=hash_val;

                    new_im->prev=ptr_t->prev;
                    new_im->next=ptr_t;
                    ptr_t->prev->next=new_im;
                    ptr_t->prev=new_im;

                    insert_update_im(updated, time, new_im, hash_val);
                }
            }
        }
        if(magazine[hash_val]->prev->expiration<magazine[hash_val]->expiration){
            magazine[hash_val]=magazine[hash_val]->prev;
        }
    }
}

void check_wo(t_ingred_mag** magazine, t_order** wo_head, t_order** wo_tail, t_order** ro_head, t_order** ro_tail, int time, t_name_check** already_checked, t_update_im** updated){
    
    unsigned short int flag=1, flag2=1, hash_val;
    t_order* ptr_o=*wo_head;
    t_name_check *ptr_hn=NULL, *ptr_tn=NULL;
    
    while(ptr_o!=NULL){
        hash_val=ptr_o->recipe->hash_val;
        flag=1;
        flag2=1;
        //controllo se già presente in already_checked
        if(already_checked[hash_val]!=NULL){
            ptr_hn=already_checked[hash_val];
            ptr_tn=already_checked[hash_val]->prev;
            //controlla se l'ordine in attesa è già stato controllato. caso 0: già controllato e non si può fare; caso 1: trovato da ptr_h, da controllare; caso 2: trovato fa ptr_t, da controllare; caso 3: non trovato, da controllare
            do{
                if(strcmp(ptr_hn->order->recipe->name,ptr_o->recipe->name)==0){
                    //l'ho trovato, se ha quantità minore esco, else controllo ingredienti. se non li ho modifico valore di already_checked->quantity
                    if(ptr_hn->quantity<=ptr_o->quantity){
                        flag=0;                        
                    }
                    else{
                        flag2=0;
                    }
                    break;
                }
                else if(strcmp(ptr_tn->order->recipe->name,ptr_o->recipe->name)==0){
                    //l'ho trovato, se ha quantità minore esco, else controllo ingredienti. se non li ho modifico valore di already_checked->quantity
                    if(ptr_tn->quantity<=ptr_o->quantity){
                        flag=0;
                    }
                    else{
                        flag2=0;
                        ptr_hn=ptr_tn;
                    }
                    break;
                }
                ptr_hn=ptr_hn->next;
                ptr_tn=ptr_tn->prev;
            }while(ptr_hn!=ptr_tn && ptr_hn!=ptr_tn->next);
        }

        if(ptr_o->missing_ptr!=NULL && ptr_o->miss_time>ptr_o->missing_ptr->time_unit){
            flag=0;
        }
        
        if(flag){

            t_order* succ=ptr_o->next;
            
            if(check_ingred(magazine, ptr_o, time, updated)){
                // rimozione da wo list
                remove_order(wo_head, wo_tail, ptr_o);

                //inserimento in ro list
                move_order(ro_head, ro_tail, ptr_o);

                //decrementa ingredienti in magazzino
                decrease_ingr(magazine, ptr_o);
            }
            else{
                if(flag2){
                    insert_name_check(already_checked, ptr_o);
                }
                else{
                    if(ptr_hn->quantity>ptr_o->quantity){
                        ptr_hn->quantity=ptr_o->quantity;
                    }
                }
            }
            ptr_o=succ;
        }
        else{
            ptr_o=ptr_o->next;
        }          
    }
}

short int check_ingred(t_ingred_mag** magazine, t_order* ord, int time, t_update_im** updated){
    
    t_ingred_rec* ptr_ir=ord->recipe->head;
    t_list* list=NULL, *last=NULL;

    do{
        short int hash_val=ptr_ir->hash_val, tmp_quant=ord->quantity*ptr_ir->quantity;
        t_ingred_mag* ptr_im=magazine[hash_val];

        if(ptr_im!=NULL){
            
            do{
                if(ptr_im->expiration>time && strcmp(ptr_im->name, ptr_ir->name)==0){
                    t_list* new=malloc(sizeof(t_list));
                    new->ingred=ptr_im;
                    new->next=NULL;
                    if(ptr_im->quantity>=tmp_quant){
                        new->quant=tmp_quant;
                    }
                    else{
                        new->quant=ptr_im->quantity;  
                    }
                    tmp_quant-=new->quant;

                    if(list==NULL){
                        list=new;
                        last=list;
                    }
                    else{
                        last->next=new;
                        last=new;
                    }
                }
                ptr_im=ptr_im->next;
            }while(tmp_quant>0 && ptr_im!=magazine[hash_val]);
        }

        if(tmp_quant>0){
            //free list
            t_list* ptr1=list, *ptr2;
            while(ptr1!=NULL){
                ptr2=ptr1;
                ptr1=ptr1->next;
                free(ptr2);
            }

            ord->recipe->head=ptr_ir;
            
            //cerco in update se trovo l'ingred, else salvo il nome
            if(updated[hash_val]!=NULL && ptr_im!=NULL){
                
                t_update_im* ptr_hu=updated[hash_val], *ptr_tu=updated[hash_val]->prev;

                if(!(ptr_hu==ptr_tu && strcmp(ptr_hu->name, ptr_ir->name)!=0)){
                    short int val_h, val_t;
                    do{
                        val_h=strcmp(ptr_hu->name, ptr_ir->name);
                        if(val_h==0){
                            break;
                        }
                        val_t=strcmp(ptr_tu->name, ptr_ir->name);
                        if(val_t==0){
                            ptr_hu=ptr_tu;
                            break;
                        }
                        ptr_hu=ptr_hu->next;
                        ptr_tu=ptr_tu->prev;
                    }while(val_h<0 && val_t>0 && ptr_hu!=ptr_tu->next);
                    if(val_h==0 || val_t==0){
                        ord->missing_ptr=ptr_hu;
                        ord->miss_time=time;
                    }
                }                
            }
            return 0;
        }
        ptr_ir=ptr_ir->next;
    }while(ptr_ir!=ord->recipe->head);

    ord->list=list;

    return 1;
}

void decrease_ingr(t_ingred_mag** magazine, t_order* ord){

    t_list* ptr=ord->list, *ptr1;

    while(ptr!=NULL){
        ptr1=ptr;
        ptr=ptr->next;
        if(ptr1->ingred->quantity==ptr1->quant){
            remove_ingr(magazine, ptr1->ingred);
        }
        else{
            ptr1->ingred->quantity-=ptr1->quant;
        }
        free(ptr1);
    }
    ord->list=NULL;
    ord->miss_time=0;
    ord->missing_ptr=NULL;
}

void remove_ingr(t_ingred_mag **magazine, t_ingred_mag* ptr_im){

    unsigned short int hash_val=ptr_im->hash_val;
    
    if(ptr_im->next==ptr_im){
        magazine[hash_val]=NULL;
    }
    else{
        if(ptr_im==magazine[hash_val]){
            magazine[hash_val]=ptr_im->next;
        }
        ptr_im->next->prev=ptr_im->prev;
        ptr_im->prev->next=ptr_im->next;
    }
    free(ptr_im);
}

void check_ro(t_ingred_mag** magazine, t_recipe* ptr_r, t_order** wo_head, t_order** wo_tail, t_order** ro_head, t_order** ro_tail, t_name_check** already_checked, t_order* new_o, int quantity_req, int time, t_update_im** updated){
    
    t_name_check* ptr_hn=NULL, *ptr_tn=NULL;
    unsigned short int flag=1, flag2=1, hash_val=new_o->recipe->hash_val;

    if(already_checked[hash_val]!=NULL){
        ptr_hn=already_checked[hash_val];
        ptr_tn=already_checked[hash_val]->prev;
        //controlla se l'ordine in attesa è già stato controllato. caso 0: già controllato e non si può fare; caso 1: trovato da ptr_h, da controllare; caso 2: trovato fa ptr_t, da controllare; caso 3: non trovato, da controllare
        do{
            if(strcmp(ptr_hn->order->recipe->name,new_o->recipe->name)==0){
                //l'ho trovato, se ha quantità minore esco, else controllo ingredienti. se non li ho modifico valore di already_checked->quantity
                if(ptr_hn->quantity<=new_o->quantity){
                    flag=0;
                }
                else{
                    flag2=0;
                }
                break;
            }
            else if(strcmp(ptr_tn->order->recipe->name,new_o->recipe->name)==0){
                //l'ho trovato, se ha quantità minore esco, else controllo ingredienti. se non li ho modifico valore di already_checked->quantity
                if(ptr_tn->quantity<=new_o->quantity){
                    flag=0;
                }
                else{
                    ptr_hn=ptr_tn;
                    flag2=0;
                }
                break;
            }
            ptr_hn=ptr_hn->next;
            ptr_tn=ptr_tn->prev;
        }while(ptr_hn!=ptr_tn && ptr_hn!=ptr_tn->next);
    }
    
    if(flag){
        
        if(check_ingred(magazine, new_o, time, updated)){

            //inserimento in ro list
            insert_order(ro_head, ro_tail, new_o);

            //decrementa ingredienti in magazzino
            decrease_ingr(magazine, new_o);
        }
        else{

            insert_order(wo_head, wo_tail, new_o);

            if(flag2){
                insert_name_check(already_checked, new_o);
            }
            else{
                ptr_hn->quantity=new_o->quantity;
            }
        }    
    }
    else{
        //inserimento in wo list

        if(ptr_hn->order->missing_ptr!=NULL){
            new_o->missing_ptr=ptr_hn->order->missing_ptr;
            new_o->miss_time=ptr_hn->order->miss_time;
        }

        insert_order(wo_head, wo_tail, new_o);
    }
}

void insert_name_check(t_name_check** already_checked, t_order* ptr_o){
                        
    t_name_check* new=malloc(sizeof(t_name_check));
    new->quantity=ptr_o->quantity;
    new->order=ptr_o;
    unsigned short int hash_val=new->order->recipe->hash_val;
    if(already_checked[hash_val]==NULL){
        new->prev=new;
        new->next=new;
        already_checked[hash_val]=new;
    }
    else{
        new->next=already_checked[hash_val];
        new->prev=already_checked[hash_val]->prev;
        already_checked[hash_val]->prev->next=new;
        already_checked[hash_val]->prev=new;
        already_checked[hash_val]=new;
    }
}