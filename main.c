#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

/*
	Allowables: malloc, calloc, realloc, free, fopen, fclose,
				getline, fscanf, fputs, fprintf, stderr, stdout, stdin, errno
*/

// DATA STRUCT

typedef struct { size_t x, y; } V2;

typedef struct {
	char em, ob, fu;
	char ** lines;
	size_t h, w, size;
	size_t **dp; V2 max;
} Data;

// FREE LINES
void	free_lines(char **lines, size_t h)
{
	if (!lines || h <= 0) return ;
	for (size_t i = 0; i < h; i++)
		free(lines[i]);
	free(lines);
}

// ERR MESSAGE
void err_msg(char *msg)
{
	fprintf(stderr, "\tError: %s\n", msg);
}

void	print_dp(size_t **dps, size_t h, size_t w)
{
	for (size_t i = 0; i < h; i++)
	{
		for (size_t j = 0; j < w; j++)
		{
			fprintf(stdout, "%zu", dps[i][j]);
		}
		fprintf(stdout,"\n");
	}
}

int helper_exit(char *buf, Data *d, char *msg)
{
	if (buf)
		free(buf);
	if (d->lines && d->h > 0)
		free_lines(d->lines, d->h);
	err_msg(msg);
	return 0;
}


// UTILITIES
char	*ft_strdup(char *s, size_t len)
{
	if (!s || len <= 0) return NULL;

	char *dup = calloc(len + 1, sizeof *dup);
	if (!dup) return NULL;
	for (size_t i = 0; i < len; i++)
		dup[i] = s[i];
	dup[len] = '\0';
	return dup;
}

size_t	strlen_no_nl(char *s)
{
	size_t i = 0;

	while (s[i] && s[i] != '\n') i++;
	return i;
}

int	all_diff(char a, char b, char c)
{
	return (a != b && b != c && a != c);
}

void	print_lines(char **lines, size_t h)
{
	if (!lines || h <= 0) return ;
	for (size_t i = 0; i < h; i++)
		fprintf(stdout, "%s\n", lines[i]);
}


// CALCULATE DYNAMIC PROGRAMMING

size_t	min_plus_one(Data *d, size_t i, size_t j)
{
	size_t left = d->dp[i][j - 1];
	size_t diag = d->dp[i - 1][j - 1];
	size_t up = d->dp[i - 1][j];
	size_t min = (left < diag) ? left : diag;
	if (min > up) min = up;
	return min + 1u;
}

void	calc_dp(Data *d)
{
	if (!d->lines) return ;

	for (size_t i = 0; i < d->h; i++)
	{
		for (size_t j = 0; j < d->w; j++)
		{
			char c = d->lines[i][j];
			if (i == 0 || j == 0)
			{
				if (c == d->em)
					d->dp[i][j] = 1u;
				else if (c == d->ob)
					d->dp[i][j] = 0u;
			}
			else if (c == d->ob)
				d->dp[i][j] = 0u;
			else
				d->dp[i][j] = min_plus_one(d, i, j);
		}
	}
}

// CALCULATE BIGGEST SQUARE

void	calc_bsq(Data *d, size_t h, size_t w)
{
	if (!d->lines) return ;

	for (size_t i = 0; i < h; i++)
	{
		for (size_t j = 0; j < w; j++)
		{
			if (d->dp[i][j] > d->size)
			{
				d->size = d->dp[i][j];
				d->max.y = i;
				d->max.x = j;
			}
		}
	}
}

// FILL BIGGIEST SQUARE
void	fill_bsq(Data *d)
{
	if (!d->lines || !d->dp) return ;

	for (size_t i = (d->max.y - d->size + 1); i <= d->max.y; i++)
		for (size_t j = (d->max.x - d->size + 1); j <= d->max.x; j++)
			d->lines[i][j] = d->fu;
}

// PROCESS FILE

int	valid_header(FILE *f, Data *d)
{
	if (fscanf(f, "%zu%c%c%c\n",
		&d->h, &d->em, &d->ob, &d->fu) != 4)
		err_msg("\tinvalid header read");
//	fprintf(stdout, "\tChars:\tHeight: %zu, Empty: %c, Obstable: %c, Full: %c\n",
	//	d->h, d->em, d->ob, d->fu);
	if (!all_diff(d->em, d->ob, d->fu))
		return helper_exit(NULL, d, "header chars not all different");
		return 1;
}

int	valid_char(char *buf, Data *d)
{
	if (!buf) return 0;
	for (size_t i = 0; buf[i] && buf[i] != '\n'; i++)
	{
		char c = buf[i];
		if (c != d->em && c != d->ob && c != d->fu) return 0;
	}
	return 1;
}

int valid_lines(FILE *f, Data *d)
{
	char	*buf = NULL;
	size_t	i = 0, first = 1, cap = 0;
	ssize_t	nread;

	while ((nread = getline(&buf, &cap, f)) > 0)
	{
		size_t	w = strlen_no_nl(buf);
		if (first) { first = 0; d->w = w; }
		if (d->w != w) return helper_exit(buf, d, "map not rectangle");
		if (i >= d->h) return helper_exit(buf, d, "map line count is more than height");
		if (!valid_char(buf, d)) return helper_exit(buf, d, "map has invalid chars");
		d->lines[i] = ft_strdup(buf, d->w);
		if (!d->lines[i]) return helper_exit(buf, d, "ft_strdup failed");
		i++;
	}
	free(buf);
	return 1;
}

int	processf(FILE *f)
{
	if (!f) return 0;

	Data d = {0};
	
	if (!valid_header(f, &d)) helper_exit(NULL, &d, "invalid header");
	d.lines = calloc(d.h + 1, sizeof d.lines);
	if (!d.lines) helper_exit(NULL, &d, "failed lines calloc");
	if (!valid_lines(f, &d)) helper_exit(NULL, &d, "invalid lines");
	d.dp = calloc(d.h, sizeof d.dp);
	if (!d.dp) helper_exit(NULL, &d, "failed d->dp calloc");
	for (size_t i = 0; i < d.h; i++)
	{
		d.dp[i] = calloc(d.w, sizeof d.dp[i]);
		if (!d.dp[i]) helper_exit(NULL, &d, "failed lines[i] calloc"); 

	}
//	print_lines(d.lines, d.h);
	calc_dp(&d);
//	print_dp(d.dp, d.h, d.w);
	calc_bsq(&d, d.h, d.w);
	fill_bsq(&d);
	print_lines(d.lines, d.h);
	free_lines(d.lines, d.h);
	return 1;
}

int main(int ac, char **av)
{
	int rc = 0;
	if (ac > 1)
	{
		for (int i = 1; i < ac; i++)
		{
			FILE *f = fopen(av[i], "r");

			if (!f) { err_msg("no file."); rc = 1; continue; }
			if (!processf(f)) { err_msg("no file processed."); rc = 2; }
			if (f) fclose(f);
		}
	}
	else
		if (!processf(stdin)) { err_msg("no stdin processed."); rc = 3; }
	return rc;
}
