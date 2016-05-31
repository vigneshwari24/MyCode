
#include<stdio.h>
#include<stdint.h>
#include<stdlib.h>
#include<string.h>
#include<ctype.h>
#define max_length 10000
#define true 1
#define false 0
#define equal -1

typedef struct bNumber
{
	char number[max_length];
	int length;
}BNUMBER;
BNUMBER SixteenPowers[200];
int leftshift(BNUMBER *a, int b);
int  subtract(BNUMBER *a, int aCounter , BNUMBER *b,int bCounter,BNUMBER *c, int cCounter , int borrow);
int CompareBnumber(BNUMBER * a, BNUMBER *b);
void makezero(BNUMBER *a);

void ConvertToBnumber(BNUMBER *a,char *b)
{
	int i = 0, j = 0;
	a->length = strlen(b);

	for(i = a->length - 1,j = 0 ; i >=0; i--,j++){
		a->number[j] = b[i] - '0';
	}
}

void printBNumber(BNUMBER *a)
{
	int i = 0;
	for(i = a->length -1; i >=0 ; i--){
		printf("%d",a->number[i]);
	}
	printf("\r\n");
}
void copy_number(BNUMBER *a, BNUMBER *b)
{
	    int i = 0;
		for(i = a->length -1; i >=0 ; i--){
		b->number[i]=a->number[i];
		b->length=a->length;
		}
}
void RemoveMSBZero(BNUMBER *a)
{
	int i = 0;
	int length = a->length;
	for(i = a->length - 1 ; i >=0;i--){
		if(a->number[i] == 0){
			a->length--;
		}
		else
			break;
	}
	if(a->length == 0){
		a->number[i] = 0;
		a->length = 1;
	}
}
int  subtract(BNUMBER *a, int aCounter , BNUMBER *b, int bCounter,BNUMBER *c, int cCounter , int borrow){
	if(CompareBnumber(a,b) == false){
		printf("a value should be greater");
		return false;
	}
	if(aCounter < a->length && aCounter < b->length){
		int op1 = a->number[aCounter] - borrow;
		int op2 = b->number[bCounter];
		if(op2 > op1)
		{
			op1 = op1 + 10;
			borrow = 1;
		}
		else
			borrow = 0;
		c->number[cCounter] = op1 - op2;
		c->length = cCounter+1;
		subtract(a,aCounter + 1, b , bCounter + 1, c, cCounter + 1,borrow);
	}
	else if(aCounter < a->length){
		int op1 = a->number[aCounter] - borrow;
		c->number[cCounter] = op1;
		c->length = cCounter+1;
		borrow = 0;
		subtract(a,aCounter + 1, b , bCounter, c, cCounter + 1, 0);
	}
	RemoveMSBZero(c);
}

int CompareBnumber(BNUMBER * a, BNUMBER *b)
{
	int i =0;
	if(a->length > b->length){
		return true;
	}
	else if(a->length < b->length){
		return false;
	}
	else {
		for(i = a->length -1; i >= 0; i--){
			if(a->number[i] > b->number[i]){
				return true;
			}
			else if(a->number[i] < b->number[i]){
				return false;
			}
		}
	}
	return -1;
}
void divide(BNUMBER *a , BNUMBER *b, BNUMBER *c,BNUMBER *m)
{
	BNUMBER r ;
	BNUMBER tmp;
	char str[1000];
	int i = 0, j = 0;
	makezero(c);
	makezero(&r);
	makezero(&tmp);

	c->length = a->length;
	for(i = a->length - 1; i>=0;i--){
		leftshift(&r , 1);


		r.number[0] = a->number[i];
		c->number[i] = 0;
		RemoveMSBZero(&r);
		while(CompareBnumber(&r,b) == true || CompareBnumber(&r,b) == equal){
			c->number[i]++;
			subtract(&r,0,b,0,&tmp,0,0);
			RemoveMSBZero(&tmp);
			r = tmp;

		}

		*m = r;
	}
	RemoveMSBZero(m);
	RemoveMSBZero(c);
}
void add(BNUMBER * a,int aCounter, BNUMBER *b,int bCounter, BNUMBER *c,int cCounter,int carry)
{
	if(aCounter < a->length && aCounter < b->length){
			int iTemp = a->number[aCounter] + b->number[bCounter] + carry;
			c->number[cCounter] = iTemp %10;
			c->length = cCounter+1;
			carry = iTemp / 10;
			add(a,aCounter + 1, b , bCounter + 1, c, cCounter + 1, carry);
		}
		else if(bCounter < b->length){
			c->number[cCounter] = b->number[bCounter] + carry;
			c->length = cCounter+1;
			add(a,aCounter, b , bCounter + 1, c, cCounter + 1, 0);
		}
		else if(aCounter < a->length){
			c->number[cCounter] = a->number[aCounter] + carry;
			c->length = cCounter+1;
			add(a,aCounter + 1, b , bCounter, c, cCounter + 1, 0);
		}
		else if(carry != 0){
			c->number[cCounter] = carry;
			c->length = cCounter + 1;
		}
}
int leftshift(BNUMBER *a, int b){
	int i = 0;
	for (i=a->length - 1; i>=0; i--)
		a->number[i+b] = a->number[i];

	for (i=0; i<b; i++)
		a->number[i] = 0;

	a->length += b;
	RemoveMSBZero(a);
}
void makezero(BNUMBER *a)
{
	memset(a,0x00,sizeof(BNUMBER));
	a->length = 1;
	a->number[0] = 0;
}
void multiply(BNUMBER * a, BNUMBER *b , BNUMBER *c)
{

		BNUMBER r ;
		BNUMBER tmp;
		makezero(c);
		makezero(&tmp);
		int i = 0, j = 0;
		r = *a;
		for(i = 0; i <= b->length-1; i++){
			for(j = 1; j <= b->number[i];j++){
				add(c,0,&r,0,&tmp,0,0);
				*c = tmp;

			}

			leftshift(&r , 1);
		}
		RemoveMSBZero(c);


}
BNUMBER convertDecToHex(BNUMBER *a,char str[1000])
{
	int i=0,j=0;
	char s[3];
	BNUMBER zero,q,rem,b,m,ans,temp,res,t;
	ConvertToBnumber(&b,"16");
	divide(a,&b,&q,&m);
	if(m.length>1)
	{
	modifyVal(&m,&t,str);
	copy_number(&t,&ans);
	}
	else
	{
		copy_number(&m,&ans);
		s[0]=m.number[0]+'0';
		s[1]='\0';
		strcpy(str,s);
	}

	ConvertToBnumber(&zero,"0");
	while(1)
	{
		makezero(&temp);
		makezero(&m);
        divide(&q,&b,&temp,&m);

		copy_number(&temp,&q);
        BNUMBERconcat(&ans,&m,&res,str);
	    copy_number(&res,&ans);
        if(CompareBnumber(&temp,&zero) == equal)
            break;
	}
 strrev(str);
 return res;
}

void BNUMBERconcat(BNUMBER *a,BNUMBER *b,BNUMBER *res,char str[])
{

	int i=0,j=0;
	char s[3];
	BNUMBER t;

	if(b->length>1)
	{
	modifyVal(b,&t,str);
	}
	else
	{
        s[0]=b->number[0]+'0';
        s[1]='\0';
		strcat(str,s);
	}

    res->length=a->length+b->length;

	j=res->length-1;
	for(i = b->length -1; i >=0 ; i--){
			res->number[j]=b->number[i];
			j--;

	}
	for(i = a->length -1; i >=0 ; i--){
				res->number[j]=a->number[i];
				j--;


		}

}
void modifyVal(BNUMBER * a,BNUMBER * r,char str[])
{

	BNUMBER A,B,C,D,E,F;
	ConvertToBnumber(&A,"10");
	ConvertToBnumber(&B,"11");
	ConvertToBnumber(&C,"12");
	ConvertToBnumber(&D,"13");
	ConvertToBnumber(&E,"14");
	ConvertToBnumber(&F,"15");

	 if(CompareBnumber(a,&A) == equal)
	 {
		 strcat(str,"A");

	 }
	 else if(CompareBnumber(a,&B) == equal)
	 {
		 strcat(str,"B");
	 }
	 else if(CompareBnumber(a,&C) == equal)
	 {
		 strcat(str,"C");
	 }
	 else if(CompareBnumber(a,&D) == equal)
	 {
		 strcat(str,"D");
	 }
	 else if(CompareBnumber(a,&E) == equal)
	 {
		 strcat(str,"E");
	 }
	 else if(CompareBnumber(a,&F) == equal)
	 {
		 strcat(str,"F");
	 }

}
BNUMBER hexToDec( char modulus[max_length] )
{
		char sHex[] = "16";
    char iString[] ="1";

	int i=0,j=0;
	BNUMBER a,hexVal,res,final,init,b,c,temp,zero;
	int len=strlen(modulus);
	ConvertToBnumber(&zero,"0");
	for(i=0;i<len;i++)
	{
		makezero(&c);
		ConvertToBnumber(&a,sHex);
		ConvertToBnumber(&b,iString);
		if(CompareBnumber(&SixteenPowers[i],&zero) == equal)
		{
			for(j=1;j<=i;j++)
			{
				multiply(&a,&b,&c);
				copy_number(&c,&b);
				//b = c; //change 1
			}
			SixteenPowers[i] = c;
		}
		else
		{
			c = SixteenPowers[i];
		}

		        if(modulus[i]>='0'&&modulus[i]<='9')
		           {
		        	   hexVal.number[0]=modulus[i]-'0';
		        	   hexVal.length=1;
		           }
		           else if(modulus[i]>='a'&&modulus[i]<='f')
		           {
		        	   hexVal.length=2;
		        	   hexVal.number[0]=modulus[i]-'a';
		        	   hexVal.number[1]=1;
		           }
		           else if(modulus[i]>='A'&&modulus[i]<='F')
		              {
		           	   hexVal.length=2;
		           	   hexVal.number[0]=modulus[i]-'A';
		           	   hexVal.number[1]=1;
		              }
		            if(i==0)
		            {
		         	   ConvertToBnumber(&init,iString);
		         	   multiply(&hexVal,&init,&final);
		         	   copy_number(&final,&temp);

		            }
		            else
		            {

		            multiply(&c,&hexVal,&res);
		            add(&res,0,&temp,0,&final,0,0);
		            copy_number(&final,&temp);
		            }

		        }


	return final;
	}

BNUMBER modInverse(BNUMBER a,BNUMBER b)
{	BNUMBER tempproduct;
	int newlength;
	int flag = 1;
	BNUMBER inverse, checkone;
	BNUMBER temp_a, temp_a1, temp_b, temp_b1, temp_mod1, temp_mod2, quotient;
	ConvertToBnumber(&temp_a, "1");
	temp_a1 = a;
	ConvertToBnumber(&temp_b, "0");
	temp_b1 = b;
	ConvertToBnumber(&checkone, "1");
	BNUMBER checkzero;
	ConvertToBnumber(&checkzero, "0");

	while(CompareBnumber(&temp_b1,&checkzero)!= equal)
	{
		divide(&temp_a1, &temp_b1, &quotient, &temp_mod2);
		multiply(&quotient, &temp_b, &tempproduct);
		add(&temp_a,0, &tempproduct,0, &temp_mod1,0,0);
		temp_a =temp_b;
		temp_b = temp_mod1;
		temp_a1 = temp_b1;
		temp_b1 = temp_mod2;
		flag = -flag;
	}
	RemoveMSBZero(&temp_a1);
	if (CompareBnumber(&temp_a1,&checkone)!= equal) return checkzero;
	if (flag < 0) subtract(&b, 0,&temp_a, 0,&inverse,0,0);
	else inverse = temp_a;
	return inverse;

}

void main()
{

    int newlength,i=0;
	char n1[max_length],n2[max_length],n3[max_length],test[100],c1[max_length],c2[max_length],c3[max_length],test1[100],finalHex[1000],msg[100];
	for(i =0 ; i < 200 ; i++)
		makezero(&SixteenPowers[i]);
    BNUMBER n1Dec,n2Dec,n3Dec,c1Dec,c2Dec,c3Dec,N,temp,N1,N2,N3,d1,d2,d3,prod1,prod2,prod3,tprod1,tprod2,tprod3,addVal,tadd,m,a,b,q,rem,x,res;

    strcpy(n1,"009623511e6769644d693e89f692ffc2558eef121d42ca98699781e139e29c2e1aa58d8883bbdba41165fdeb85a9a5648fc29a65d59e9401694dd11ae205f0ce3b");
    strcpy(n2,"00ad4bc0f980f4523f490fc40c12efcecc1e8af67890b6562449876e8e091e861cda699e5a8eb309b0a9d6b293100c1229fbd18a5951f33b6fbab1fd8d90f7c829");
    strcpy(n3,"00b7223364d88353ec02b0850e8a01d2ba9ca2663c32c15df7b596406c6fc1c171ac965a554b8b338f4bb046c543937b4b19c699864f1d0dd4be0177eccce0bb57");

    strcpy(c1,"34d2fc2fa4785e1cdb1c09c9a5db98317d702aaedd2759d96e8938f740bf982e2a42b904e54dce016575142f1b0ed112cc214fa8378b0d5eebc036dc7df3eeea");
    strcpy(c2,"3ddd68eeff8be9fee7d667c3c0ef21ec0d56cefab0fa10199c933cffbf0924d486296c604a447f48b9f30905ee49dd7ceef8fc689a1c4c263c1b3a9505091b00");
    strcpy(c3,"956f7cbf2c9da7563365827aba8c66dc83c9fb77cf7ed0ca225e7d155d2f573d6bd18e1c18044cb14c59b52d3d1f6c38d8941a1d58942ed7f13a52caccc48154");


    strrev(n1);
    strrev(n2);
    strrev(n3);

    strrev(c1);
    strrev(c2);
    strrev(c3);

    n1Dec= hexToDec(n1);
    printf("n1 in Hex: \n%s\n n1 in decimal \n ",strrev(n1));
    printBNumber(&n1Dec);

    n2Dec= hexToDec(n2);
    printf("n2 in Hex: \n%s\n n2 in decimal \n ",strrev(n2));
    printBNumber(&n2Dec);

    n3Dec= hexToDec(n3);
    printf("n3 in Hex: \n%s\n n3 in decimal \n ",strrev(n3));
    printBNumber(&n3Dec);

    c1Dec= hexToDec(c1);
    printf("c1 in Hex: \n%s\n c1 in decimal \n ",strrev(c1));
    printBNumber(&c1Dec);

     c2Dec= hexToDec(c2);
     printf("c2 in Hex: \n%s\n c2 in decimal \n ",strrev(c2));
     printBNumber(&c2Dec);

     c3Dec= hexToDec(c3);
     printf("c3 in Hex: \n%s\n c3 in decimal \n ",strrev(c3));
     printBNumber(&c3Dec);

     multiply(&n1Dec,&n2Dec,&temp);
     multiply(&n3Dec,&temp,&N);

     printf("\n Value of N \n");
     printBNumber(&N);

     divide(&N,&n1Dec,&N1,&m);
     printf("\n Value of N1 \n");
     printBNumber(&N1);

     makezero(&m);
     divide(&N,&n2Dec,&N2,&m);
     printf("\n Value of N2 \n");
     printBNumber(&N2);

     makezero(&m);
     divide(&N,&n3Dec,&N3,&m);
     printf("\n Value of N3 \n");
     printBNumber(&N3);


     d1=modInverse(N1,n1Dec);
     printf("\n Value of d1 \n");
     printBNumber(&d1);

     d2=modInverse(N2,n2Dec);
     printf("\n Value of d2 \n");
     printBNumber(&d2);

     d3=modInverse(N3,n3Dec);
     printf("\n Value of d3 \n");
     printBNumber(&d3);

     multiply(&c1Dec,&N1,&tprod1);
     multiply(&tprod1,&d1,&prod1);
     printf("\n Value of prod1 \n");
     printBNumber(&prod1);

     multiply(&c2Dec,&N2,&tprod2);
     multiply(&tprod2,&d2,&prod2);
     printf("\n Value of prod2 \n");
     printBNumber(&prod2);

     multiply(&c3Dec,&N3,&tprod3);
     multiply(&tprod3,&d3,&prod3);
     printf("\n Value of prod3 \n");
     printBNumber(&prod3);

     add(&prod1,0,&prod2,0,&tadd,0,0);
     add(&tadd,0,&prod3,0,&addVal,0,0);

     divide(&addVal,&N,&q,&m);
     printf("\n Value of m cube \n");
     printBNumber(&m);

	 newlength = (m.length - 1) / 3 + 1;
	 printf("the new length is %d", newlength);
	char lowerbound[newlength+1], upperbound[newlength +2];
	lowerbound[0] = '1';
	upperbound[0] = '1';
	for(i = 1; i < newlength; i++) {
		lowerbound[i] = '0';
		upperbound[i] = '0';
	}
	upperbound[newlength] = '0';
	upperbound[newlength + 1] = '\0';
	lowerbound[newlength] = '\0';
	BNUMBER lb,ub;
	ConvertToBnumber(&lb, lowerbound);
	ConvertToBnumber(&ub, upperbound);
	BNUMBER two;
	ConvertToBnumber(&two,"2");
	BNUMBER message;
	while(1) {
		BNUMBER tempadd;
		BNUMBER temp,remainder;
		add(&lb,0,&ub,0,&tempadd,0,0);
		divide(&tempadd,&two,&temp,&remainder);
		BNUMBER tempmul;
		multiply(&temp,&temp,&tempmul);
		BNUMBER tempcube;
		multiply(&temp, &tempmul, &tempcube);
		if(CompareBnumber(&tempcube,&m) == equal) {
			message = temp;
			//exit(0);
			break;
		} else if(CompareBnumber(&tempcube,&m) == true){
			ub = temp;
		} else {
			lb = temp;
		}

	}
	 printf("\n Plain Text Message in Decimal \n");
     printBNumber(&message);
     res=convertDecToHex(&message,finalHex);
	 int k = 0;
	 for( i = 0; i < strlen(finalHex); i += 2) {
		char buff[3];
		buff[0] = finalHex[i];
		buff[1] = finalHex[i+1];
		char m[2];
		sprintf(m,"%c",strtol(buff, NULL, 16));
		msg[k++] = m[0];
	 }
	 msg[k] = '\0';
	 printf("THE DECRYPTED PALIN TEXT IS : \n \"%s\"",msg);
}

