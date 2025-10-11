#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// Data struct

typedef struct {
	size_t x;
	size_t y;
} V2;

typedef struct {
	char	em, ob, fu;
	char	**lines;
	size_t	h, w, size;
	size_t	**dp;
	V2		max;
} Data;

// error messages

void	err_msg(char *msg)
{
	fprintf(stderr, "%s\n", msg);
}

// Line Utils

void	free_lines(char **lines, size_t h)
{
	if (!lines || h <= 0) return ;
	for (size_t i = 0; i < h; i++)
		free(lines[i]);
	free(lines);
}

void	print_lines(char **lines, size_t h)
{
	if (!lines || h <= 0) return ;
	for (size_t i = 0; i < h; i++)
		fprintf(stdout, "%s\n", lines[i]);
}

// Valid Header

int	all_diff(char a, char b, char c)
{
	return (a != b && a != c && b != c);
}

int	valid_header(FILE *f, Data *d)
{
	char sp1[2] = "", sp2[2] = "", sp3[2] = "", nl[2] = "";

	if (fscanf(f, "%zu%1[ ]%c%1[ ]%c%1[ ]%c%1[\n]",
		&d->h, sp1, &d->em, sp2, &d->ob, sp3, &d->fu, nl) != 8)
		err_msg("invalid header read");
	fprintf(stdout, "Height: %zu, Empty: %c, Obstable: %c, Full:  %c\n",
		d->h, d->em, d->ob, d->fu);
	if (!all_diff(d->em, d->ob, d->fu)) {err_msg("not all char diff"); return 0;}
	return 1;
}

// Valid Lines Utils

size_t	strlen_no_nl(char *s)
{
	size_t i = 0;
	while (s[i] && s[i] != '\n') i++;
	return i;
}

int	valid_chars(char *buf, Data *d)
{
	if (!buf) return 0;
	for (size_t i = 0; buf[i] && buf[i] != '\n'; i++)
	{
		char c = buf[i];
		if (c != d->em && c != d->ob && c != d->fu) return 0;
	}
	return 1;
}

char *ft_strdup(char *s, size_t len)
{
	if (!s || len <= 0) return NULL;

	char *dup = calloc(len + 1, sizeof *dup);
	if (!dup) return NULL;
	for (size_t i = 0; i < len; i++)
		dup[i] = s[i];
	dup[len] = '\0';
	return dup;
}

// Valid Lines

int valid_lines(FILE *f, Data *d)
{
	char	*buf = NULL;
	size_t	i = 0, first = 1, cap = 0;
	ssize_t	nread;

	while ((nread = getline(&buf, &cap, f)) > 0)
	{
		size_t w = strlen_no_nl(buf);
		if (first)
		{
			first = 0;
			d->w = w;
		}
		if (d->w != w) { free(buf); free_lines(d->lines, d->h); err_msg("map not rect"); return 0; }
		if (i >= d->h) { free(buf); free_lines(d->lines, d->h); err_msg("map lines more than height"); return 0; }
		if (!valid_chars(buf, d)) { free(buf); free_lines(d->lines, d->h); err_msg("map has invalid char"); return 0; }
		d->lines[i] = ft_strdup(buf, d->w);
		if (!d->lines[i]) { free(buf); free_lines(d->lines, d->h); return 0; }
		i++;
	}
	free(buf);
	return 1;
}

// Calculate Dynamic Programming Utils

size_t	min_plus_one(Data *d, size_t i, size_t j)
{
	size_t left = d->dp[i][j - 1];
	size_t diag = d->dp[i - 1][j - 1];
	size_t up = d->dp[i - 1][j];
	size_t min = (left < diag) ? left : diag;
	if (min > up) min = up;
	return min + 1u;
}

// Calculate Dynamic Programming

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

// Fill Biggest Square

void	calc_bsq(Data *d)
{
	if (!d->lines) return ;
	for (size_t i = 0; i < d->h; i++)
	{
		for (size_t j = 0; j < d->w; j++)
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

void	fill_bsq(Data *d)
{
	if (!d->lines) return ;
	for (size_t i = (d->max.y - d->size + 1); i <= d->max.y; i++)
		for (size_t j = (d->max.x - d->size + 1); j <= d->max.x; j++)
			d->lines[i][j] = d->fu;			
}

// Process File

int	processf(FILE *f)
{
	if (!f) return 0;

	Data d = {0};

	if (!valid_header(f, &d)) { err_msg("invalid header"); return 0; }
	d.lines = calloc(d.h + 1, sizeof d.lines);
	if (!d.lines) { err_msg("failed d->lines calloc");  return 0; }
	if (!valid_lines(f, &d)) { err_msg("invalid lines"); return 0; }
//	print_lines(d.lines, d.h);
	d.dp = calloc(d.h, sizeof d.dp);
	if (!d.dp) { err_msg("failed d->lines calloc");  return 0; }
	for (size_t i = 0; i < d.h; i++)
	{
		d.dp[i] = calloc(d.w, sizeof d.dp[i]);
		if (!d.dp[i]) { err_msg("failed d->lines[i] calloc");  return 0; }
	}	
	calc_dp(&d);
	calc_bsq(&d);
	fill_bsq(&d);
	print_lines(d.lines, d.h);
	free_lines(d.lines, d.h);
	return 1;
}

// main

int	main(int ac, char **av)
{
	int rc = 0;
	if (ac > 1)
	{
		for (int i = 1; i < ac; i++)
		{
			FILE *f = fopen(av[i], "r");

			if (!f) { err_msg("Map Error: no file\n"); rc = 1; continue; }
			if (!processf(f)) { err_msg("Map Error: no file process\n"); rc = 2;}
			if (f) fclose(f);
		}
	}
	else
		if (!processf(stdin)) { err_msg("Map Error: no stdin process\n"); rc = 3;}
	return rc;
}
