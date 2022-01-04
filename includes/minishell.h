/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tnard <tnard@student.42lyon.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/01/03 10:47:55 by tnard             #+#    #+#             */
/*   Updated: 2022/01/03 10:47:55 by tnard            ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#ifndef MINISHELL_H
# define MINISHELL_H

# include <unistd.h>
# include "../libft/libft.h"
# include "../ft_printf/ft_printf.h"
# include "pipex.h"

typedef struct s_process
{
	struct s_process	*next;
	char				*command;
	char				*path;
	char				*args;
	char				*output;
	char				*input;
	char				*type;
}			t_process;

typedef struct s_check
{
	int	quote;
	int	squote;
	int	inside;
}			t_check;

char		*ft_get_input(int fd, int size, int len);
t_process	*ft_parse_command(char *str, char **env);
int			ft_check_quote(char *str);
int			ft_check_inout(char *str);
void		ft_free_split(char **split);
int			ft_check_inout_n(char *str);

#endif