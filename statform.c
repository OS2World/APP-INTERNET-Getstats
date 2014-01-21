/*
** statform 1.0
** A getstats form interface
** By Brian Behlendorf, gw@wired.com
** Hacked on 4/15/94 by Kevin Hughes, kevinh@eit.com
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*

WARNING! THIS FORM HAS A SECURITY BUG!
A new version will be out shortly which fixes the problem
(and others associated with this version).
I'll try to get it out soon, but in the meantime I'll 
gladly accept patches! -- Kevin

*/

/* User-definable options below */

char LOGFILE[80] = "/usr/local/httpd/logs/access_log";
	/*
	** The full path to your log file. This is used as a default.
	*/
char ROOTDIR[80] = "/usr/local/www/";
	/*
	** The top of your Web (or Gopher) tree.
	*/
char GETSTATS[80] = "/usr/local/httpd/cgi-bin/getstats";
	/*
	** The full path to getstats, version 1.2 or better.
	*/

/* End of user-definable options */

#define SUBJECT "Getstats results"
#define MAXENTRIES 10000
#define MAXCMDLEN 1024

typedef struct {
	char *name;
	char *val;
} entry;

char command[MAXCMDLEN];

char *makeword(char *line, char stop);
char *fmakeword(FILE *f, char stop, int *len);
char x2c(char *what);
void unescape_url(char *url);
void plustospace(char *str);

main(int argc, char *argv[])
{
	entry entries[MAXENTRIES];
	register int x, m = 0;
	int cl, invalid = 0, didroot = 0, didhtml = 0;
	
	char protocol;

	if (strcmp(getenv("REQUEST_METHOD"), "POST")) {
		printf("We can't do much with a blank subscription card, ");
		printf("can we?\n");
		exit(1);
	}

	if (strcmp(getenv("CONTENT_TYPE"),
	"application/x-www-form-urlencoded")) {
		printf("This script can only be used to decode form ");
		printf("results.\n");
		exit(1);
	}

	cl = atoi(getenv("CONTENT_LENGTH"));
	
	sprintf(command, "%s ", GETSTATS);

	for(x = 0; cl && (!feof(stdin)); x++) {
		m = x;
		entries[x].val = fmakeword(stdin, '&', &cl);
		plustospace(entries[x].val);
		unescape_url(entries[x].val);
		entries[x].name = makeword(entries[x].val,'=');

		if (!strcmp(entries[x].name, "protocol")) {
			protocol = entries[x].val[0];

			if (protocol == 'G') 
				sprintf(command, "%s -G %s", command);
			else if (protocol == 'N')
				sprintf(command, "%s -N %s", command);
			else if (protocol == 'C')
				sprintf(command, "%s -C %s", command);
			else if (protocol == 'P')
				sprintf(command, "%s -P %s", command);
			else if (protocol == 'M')
				sprintf(command, "%s -A %s", command);
			else if (protocol == 'U')
				sprintf(command, "%s -O %s", command);
		}

		if (!didroot) {
			sprintf(command, "%s -dr %s", command, ROOTDIR);
			didroot = 1;
		}

		if (!strcmp(entries[x].name, "logfilename"))
			if (strlen(entries[x].val)) 
				sprintf(command, "%s -l %s", command,
				entries[x].val);
			else 
				sprintf(command, "%s -l %s", command, LOGFILE);

		if (!strcmp(entries[x].name, "common") && 
		!strcmp(entries[x].val, "on"))
			sprintf(command, "%s -M", command);

		if (!strcmp(entries[x].name, "concise") && 
		!strcmp(entries[x].val, "on"))
			sprintf(command, "%s -c", command);

		if (!strcmp(entries[x].name, "toplines") && 
		strlen(entries[x].val))
			sprintf(command, "%s -t %s ", command, entries[x].val);

		if (!strcmp(entries[x].name, "all") && 
		!strcmp(entries[x].val, "on"))
			sprintf(command, "%s -a ", command);

		if (!strcmp(entries[x].name, "monthly") && 
		!strcmp(entries[x].val, "on"))
			sprintf(command, "%s -m ", command);

		if (!strcmp(entries[x].name, "weekly") && 
		!strcmp(entries[x].val, "on"))
			sprintf(command, "%s -w ", command);

		if (!strcmp(entries[x].name, "daysweek") && 
		!strcmp(entries[x].val, "on"))
			sprintf(command, "%s -ds ", command);

		if (!strcmp(entries[x].name, "daily") && 
		!strcmp(entries[x].val, "on"))
			sprintf(command, "%s -d ", command);

		if (!strcmp(entries[x].name, "hoursday") && 
		!strcmp(entries[x].val, "on"))
			sprintf(command, "%s -hs ", command);

		if (!strcmp(entries[x].name, "hourly") && 
		!strcmp(entries[x].val, "on"))
			sprintf(command, "%s -h ", command);

		if (!strcmp(entries[x].name, "full-hostname") && 
		!strcmp(entries[x].val, "on"))
			sprintf(command, "%s -f ", command);

		if (!strcmp(entries[x].name, "full-access") && 
		!strcmp(entries[x].val, "on"))
			sprintf(command, "%s -fa ", command);

		if (!strcmp(entries[x].name, "full-lastaccess") && 
		!strcmp(entries[x].val, "on"))
			sprintf(command, "%s -fd ", command);

		if (!strcmp(entries[x].name, "full-bytes") && 
		!strcmp(entries[x].val, "on"))
			sprintf(command, "%s -fb ", command);

		if (!strcmp(entries[x].name, "request-name") && 
		!strcmp(entries[x].val, "on"))
			sprintf(command, "%s -r ", command);

		if (!strcmp(entries[x].name, "request-accesses") && 
		!strcmp(entries[x].val, "on"))
			sprintf(command, "%s -ra ", command);

		if (!strcmp(entries[x].name, "request-lastaccesses") && 
		!strcmp(entries[x].val, "on"))
			sprintf(command, "%s -rd ", command);

		if (!strcmp(entries[x].name, "request-bytes") && 
		!strcmp(entries[x].val, "on"))
			sprintf(command, "%s -rb ", command);

		if (!strcmp(entries[x].name, "request-filesize") && 
		!strcmp(entries[x].val, "on"))
			sprintf(command, "%s -rf ", command);

		if (!strcmp(entries[x].name, "domain-name") && 
		!strcmp(entries[x].val, "on"))
			sprintf(command, "%s -dn ", command);

		if (!strcmp(entries[x].name, "domain-requests") && 
		!strcmp(entries[x].val, "on"))
			sprintf(command, "%s -da ", command);

		if (!strcmp(entries[x].name, "domain-lastaccesses") && 
		!strcmp(entries[x].val, "on"))
			sprintf(command, "%s -dd ", command);

		if (!strcmp(entries[x].name, "domain-bytes") && 
		!strcmp(entries[x].val, "on"))
			sprintf(command, "%s -db ", command);

		if (!strcmp(entries[x].name, "domain-sub") && 
		!strcmp(entries[x].val, "on"))
			sprintf(command, "%s -du ", command);

		if (!strcmp(entries[x].name, "filetree") && 
		!strcmp(entries[x].val, "on"))
			sprintf(command, "%s -dt ", command);

		if (!strcmp(entries[x].name, "error") && 
		!strcmp(entries[x].val, "on"))
			sprintf(command, "%s -e ", command);

		if (!strcmp(entries[x].name, "samask"))
			if (strlen(entries[x].val) != 0)
				sprintf(command, "%s -sa \"%s\"", command,
				entries[x].val);
			
		if (!strcmp(entries[x].name, "ssmask"))
			if (strlen(entries[x].val) != 0)
				sprintf(command, "%s -ss \"%s\"", command,
				entries[x].val);
			
		if (!strcmp(entries[x].name, "srmask"))
			if (strlen(entries[x].val) != 0)
				sprintf(command, "%s -sr \"%s\"", command,
				entries[x].val);
			
		if (!strcmp(entries[x].name, "spmask"))
			if (strlen(entries[x].val) != 0)
				sprintf(command, "%s -sp \"%s\"", command,
				entries[x].val);
			
		if (!strcmp(entries[x].name, "sdmask"))
			if (strlen(entries[x].val) != 0)
				sprintf(command, "%s -sd \"%s\"", command,
				entries[x].val);
			
		if (!strcmp(entries[x].name, "shmask"))
			if (strlen(entries[x].val) != 0)
				sprintf(command, "%s -sh \"%s\"", command,
				entries[x].val);
			
		if (!strcmp(entries[x].name, "swmask"))
			if (strlen(entries[x].val) != 0)
				sprintf(command, "%s -sw \"%s\"", command,
				entries[x].val);
			
		if (!strcmp(entries[x].name, "mailme") && 
		strlen(entries[x].val) != 0) {
			sprintf(command, "%s | /usr/ucb/mail -s \"%s\" %s",
			command, SUBJECT, entries[x].val);
			printf("Content-type: text/html\n\n");
			printf("<title>It's in the mail...</title>\n<p>\n");
			printf("<h1>Your log request is being mailed to ");
			printf("you.</h1>\n");
			printf("The following command was executed:\n");
			printf("<p>\n<code>%s</code>\n", command);
		}
		else if (!strcmp(entries[x].name, "mailme") &&
		strlen(entries[x].val) == 0 && !didhtml) {
			sprintf(command, "%s -ht", command);
			didhtml = 1;
		}
	}
	sprintf(command, "%s &\n", command);
	system(command);

	exit(0);
}

/* util.c from NCSA included below */

#define LF 10
#define CR 13

void getword(char *word, char *line, char stop) {
    int x = 0,y;

    for(x=0;((line[x]) && (line[x] != stop));x++)
        word[x] = line[x];

    word[x] = '\0';
    if(line[x]) ++x;
    y=0;

    while(line[y++] = line[x++]);
}

char *makeword(char *line, char stop) {
    int x = 0,y;
    char *word = (char *) malloc(sizeof(char) * (strlen(line) + 1));

    for(x=0;((line[x]) && (line[x] != stop));x++)
        word[x] = line[x];

    word[x] = '\0';
    if(line[x]) ++x;
    y=0;

    while(line[y++] = line[x++]);
    return word;
}

char *fmakeword(FILE *f, char stop, int *cl) {
    int wsize;
    char *word;
    int ll;

    wsize = 102400;
    ll=0;
    word = (char *) malloc(sizeof(char) * (wsize + 1));

    while(1) {
        word[ll] = (char)fgetc(f);
        if(ll==wsize) {
            word[ll+1] = '\0';
            wsize+=102400;
            word = (char *)realloc(word,sizeof(char)*(wsize+1));
        }
        --(*cl);
        if((word[ll] == stop) || (feof(f)) || (!(*cl))) {
            if(word[ll] != stop) ll++;
            word[ll] = '\0';
            return word;
        }
        ++ll;
    }
}

char x2c(char *what) {
    register char digit;

    digit = (what[0] >= 'A' ? ((what[0] & 0xdf) - 'A')+10 : (what[0] - '0'));
    digit *= 16;
    digit += (what[1] >= 'A' ? ((what[1] & 0xdf) - 'A')+10 : (what[1] - '0'));
    return(digit);
}

void unescape_url(char *url) {
    register int x,y;

    for(x=0,y=0;url[y];++x,++y) {
        if((url[x] = url[y]) == '%') {
            url[x] = x2c(&url[y+1]);
            y+=2;
        }
    }
    url[x] = '\0';
}

void plustospace(char *str) {
    register int x;

    for(x=0;str[x];x++) if(str[x] == '+') str[x] = ' ';
}

int rind(char *s, char c) {
    register int x;
    for(x=strlen(s) - 1;x != -1; x--)
        if(s[x] == c) return x;
    return -1;
}

int getline(char *s, int n, FILE *f) {
    register int i=0;

    while(1) {
        s[i] = (char)fgetc(f);

        if(s[i] == CR)
            s[i] = fgetc(f);

        if((s[i] == 0x4) || (s[i] == LF) || (i == (n-1))) {
            s[i] = '\0';
            return (feof(f) ? 1 : 0);
        }
        ++i;
    }
}

void send_fd(FILE *f, FILE *fd)
{
    int num_chars=0;
    char c;

    while (1) {
        c = fgetc(f);
        if(feof(f))
            return;
        fputc(c,fd);
    }
}
