/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_exec_cmd_3.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asaffroy <asaffroy@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/02/01 09:31:23 by asaffroy          #+#    #+#             */
/*   Updated: 2022/02/02 17:53:54 by asaffroy         ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

int	ft_execute_cmd_11(t_process *temp, t_data *data, int i, char **env)
{
	if (!ft_is_command(temp->command, "exit"))
	{
		write(2, "exit\n", 5);
		free_exec(data, i);
		exit(data->status);
	}
	data->tab_args[0] = ft_dquote(ft_splitd(temp->cmd_arg, ' '), 0, 0);
	if (!ft_built(0, data, temp))
		one_proc(data, temp, env);
	else
	{
		data->status = 0;
		free(data->tab_args[0]);
		free_exec(data, i);
		return (0);
	}
	data->j = 0;
	return (1);
}

int	ft_status(t_data *data)
{
	if (WIFEXITED(data->status))
		return (WEXITSTATUS(data->status));
	else if (WIFSIGNALED(data->status))
		return (WTERMSIG(data->status));
	return (data->status);
}
