/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokenizer_scan.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: naylee <naylee@student.42seoul.kr>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/11 12:09:36 by naylee            #+#    #+#             */
/*   Updated: 2023/08/11 12:09:36 by naylee           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */
#include "../include/tokenizer.h"
#define SYMBOLCHAR "<>&|() \t\n"

t_token	*scan_char_token(t_tokenizer *tokenizer)
{
	if (*tokenizer->start == '<')
	{
		if (match(tokenizer, '<'))
			return (make_token(tokenizer, DLESS));
		return (make_token(tokenizer, LESS));
	}
	if (*tokenizer->start == '>')
	{
		if (match(tokenizer, '>'))
			return (make_token(tokenizer, DGREAT));
		return (make_token(tokenizer, GREAT));
	}
	if (*tokenizer->start == '&')
	{
		if (match(tokenizer, '&'))
			return (make_token(tokenizer, AND_IF));
	}
	if (*tokenizer->start == '|')
	{
		if (match(tokenizer, '|'))
			return (make_token(tokenizer, OR_IF));
		return (make_token(tokenizer, PIPE));
	}
	return (scan_word_token(tokenizer));
}

t_token	*scan_word_token(t_tokenizer *tokenizer)
{
	int	flag;

	flag = 0;
	while (!ft_strchr(SYMBOLCHAR, *tokenizer->end))
	{
		if (*tokenizer->end == '\0')
			return (make_token(tokenizer, E0F));
		if (*tokenizer->end == '\'' || *tokenizer->end == '"')
		{
			if (string_close(tokenizer, *tokenizer->end) == FALSE)
			{
				//syntax_error
			}
			else
			{
				while (*tokenizer->end != '\'' && *tokenizer->end != '"')
					tokenizer->end++;
				flag++;
			}
		}
		tokenizer->end++;
	}
	if (tokenizer->start != tokenizer->end && ft_strchr(SYMBOLCHAR, *tokenizer->end))
		tokenizer->end--;
	if (flag)
		return (make_merge_word_token(tokenizer, flag));
	return (make_token(tokenizer, WORD));
}

t_bool	string_close(t_tokenizer *tokenizer, char c)
{
	char	*end_ptr;

	end_ptr = tokenizer->end + 1;
	while (*end_ptr != '\0' && *end_ptr != c)
		end_ptr++;
	if (*end_ptr == '\0' || *end_ptr != c)
		return (FALSE);
	if (c != ')')
		tokenizer->end = end_ptr;
	return (TRUE);
}
