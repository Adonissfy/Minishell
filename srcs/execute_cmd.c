/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute_cmd.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asaffroy <asaffroy@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/01/03 11:15:22 by asaffroy          #+#    #+#             */
/*   Updated: 2022/01/31 11:08:37 by tnard            ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/*
*int	find_path(char **env)
*env	env of the shell
*do :	find the line "PATH" in the env
*/

int	find_path(char **env)
{
	int	i;

	i = 0;
	while (env[i])
	{
		if (!ft_strncmp(env[i], "PATH=", 5))
			return (i);
		i++;
	}
	return (-1);
}

int	ft_built(int i, char **env, t_data *data, t_process *temp)
{
	if (!ft_is_command(temp->command, "pwd"))
		return (ft_pwd(1));
	if (!ft_strncmp(temp->command, "env", 3))
		return (ft_env(data, 1));
	if (!ft_strncmp(temp->command, "exit", 4))
		exit(0);
	if (!ft_is_command(temp->command, "cd"))
		return (ft_cd(data, 1, data->tab_args[i]));
	if (!ft_is_command(temp->command, "export"))
		return (ft_export(data, data->tab_args[i], 1));
	if (!ft_is_command(temp->command, "unset"))
		return (ft_unset(data, data->tab_args[i], 1));
	if (!ft_is_command(temp->command, "echo"))
		return (ft_echo(data->tab_args[i], 1));
	return (0);
}

int	ft_built_red(int i, char **env, t_data *data, t_process *temp)
{
	if (!ft_is_command(temp->command, "pwd"))
		return (ft_pwd(data->fd[2 * (data->ind + 1) + 1]));
	if (!ft_strncmp(temp->command, "env", 3))
		return (ft_env(data, data->fd[2 * (data->ind + 1) + 1]));
	if (!ft_strncmp(temp->command, "exit", 4))
		ft_exit(temp);
	if (!ft_is_command(temp->command, "cd"))
		return (ft_cd(data, data->fd[2 * (data->ind + 1) + 1], data->tab_args[i]));
	if (!ft_is_command(temp->command, "export"))
		return (ft_export
			(data, data->tab_args[i], data->fd[2 * (data->ind + 1) + 1]));
	if (!ft_is_command(temp->command, "echo"))
		return (ft_echo(data->tab_args[i], data->fd[2 * (data->ind + 1) + 1]));
	if (!ft_is_command(temp->command, "unset"))
		return (ft_unset
			(data, data->tab_args[i], data->fd[2 * (data->ind + 1) + 1]));
	return (0);
}

int	ft_built_red2(int i, char **env, t_data *data, t_process *temp)
{
	if (!ft_is_command(temp->command, "pwd"))
		return (ft_pwd(data->file[i]));
	if (!ft_strncmp(temp->command, "env", 3))
		return (ft_env(data, data->file[i]));
	if (!ft_strncmp(temp->command, "exit", 4))
		ft_exit(temp);
	if (!ft_is_command(temp->command, "cd"))
		return (ft_cd(data, data->file[i], data->tab_args[i]));
	if (!ft_is_command(temp->command, "export"))
		return (ft_export
			(data, data->tab_args[i], data->file[i]));
	if (!ft_is_command(temp->command, "echo"))
		return (ft_echo(data->tab_args[i], data->file[i]));
	if (!ft_is_command(temp->command, "unset"))
		return (ft_unset
			(data, data->tab_args[i], data->file[i]));
	return (0);
}

/*
*char	*ft_check_arg(char *cmd, char **env)
*cmd	command to search in /bin
*env	env of the shell
*error	not norminette
*do :	check if the command exist and if we have the rights to use it 
*/

char	*ft_strxjoin(char *s1, char *s2, int n)
{
	int		i;
	int		j;
	char	*str;
	int		k;

	k = 0;
	j = ft_strlen(s2) + n + 1;
	str = (char *)malloc(sizeof(char) * j);
	if (!str)
		return (NULL);
	i = -1;
	while (++i < (int)ft_strlen(s1))
		str[i] = s1[i];
	while (n-- && s2[k])
		str[i++] = s2[k++];
	str[i] = '\0';
	free(s2);
	return (str);
}

char	*ft_check_arg2(char **env, char *temp, int i)
{
	char	**tab;
	char	*try;

	tab = ft_split(env[i] + 5, ':');
	i = 0;
	while (tab[i])
	{
		try = ft_strjoin(tab[i], temp);
		if (!access(try, X_OK))
		{
			i = -1;
			while (tab[++i])
				free(tab[i]);
			free(tab);
			return (try);
		}
		i++;
	}
	free(tab);
	return (NULL);
}

char	*ft_check_arg(char *cmd, char **env)
{
	int			i;
	char		*temp;
	char		*try_temp;

	temp = ft_ddquote(ft_strdup(cmd), 0);
	if (!access(temp, F_OK))
	{
		if (ft_strlen(temp) > 2 && temp[0] == '.'
			&& temp[1] == '/' && !access(temp, X_OK))
			return (temp);
		else
			if (ft_strchr(temp, '/'))
				ft_perror("minishell: Permission denied");
	}
	if (ft_strchr(temp, '/'))
		ft_perror("minishell: No such file or directory");
	temp = ft_strxjoin("/", temp, ft_strlen(temp));
	if (find_path(env) == -1)
		ft_perror("minishell: No such file or directory");
	else
		try_temp = ft_check_arg2(env, temp, find_path(env));
	free(temp);
	if (!try_temp)
		ft_perror("minishell: No such file or directory");
	return (try_temp);
}

/*
*void	father_proc(t_data *data, char **argv, char **env)
*data	my struct with all data needed
*env	env of the shell
*argv	tab of args received at launch of prog
*do :	exec one of the cmd
*/

int	ft_malloc_struct(t_data *data, int i)
{
	int	j;

	j = -1;
	data->fd = (int *)malloc(sizeof(int) * i * 2);
	if (!data->fd)
		return (0);
	data->tab_args = malloc(sizeof(char **) * i);
	if (!data->tab_args)
		return (0);
	data->tab_paths = malloc(sizeof(char *) * i);
	if (!data->tab_paths)
		return (0);
	data->file = malloc(sizeof(int) * i);
	if (!data->file)
		return (0);
	data->charset[0] = '\'';
	data->charset[1] = '\"';
	data->charset[2] = ' ';
	data->charset[3] = '\0';
	data->nb_cmd = i;
	data->pid1 = malloc(sizeof(pid_t) * i);
	return (1);
}

void	create_pipes(t_data *data)
{
	int	i;

	i = 0;
	while (i < data->nb_cmd)
	{
		if (pipe(data->fd + 2 * i) < 0)
			ft_perror("fail , lets free\n");
		i++;
	}
}

void	close_pipes(t_data *data)
{
	int	i;

	i = 0;
	while (i < 2 * data->nb_cmd)
	{
		close(data->fd[i]);
		i++;
	}
}

void	free_exec(t_data *data, int i)
{
	int			j;

	i = 0;
	//temp = proc;
	j = 0;
	//while (++i && temp->next != NULL)
		//temp = temp->next;
	/*while (i > 0)
	{
		j = -1;
		if (temp->cmd_arg != NULL)
			free(temp->cmd_arg);
		if (temp->command != NULL)
			free(temp->command);
		if (temp->type != NULL)
			free(temp->type);
		free(temp);
		i--;
		temp = proc;
		while (++j < i - 1)
			temp = temp->next;
	}*/
	if (data->fd)
		free(data->fd);
	if (data->pid1)
		free(data->pid1);
	if (data->tab_args)
		free(data->tab_args);
	// if (data->tab_paths) //need to be free
	// {
	// 	while (data->tab_paths[j] != NULL)
	// 	{
	// 		free(data->tab_paths[j]);
	// 		ft_printf("freed\n");
	// 		j++;
	// 	}
	// }
	if (data->tab_paths) //need to be free
		free(data->tab_paths);
	// j = -1; need to be free
	// while (data->file[++j])
	// 	free(data->file[j]);
	if (data->file)
		free(data->file); // need to be free
}

void	pipe_proc_2(t_process *temp, t_data *data, int i, char **env)
{
	if (temp->in_prev != 0)
	{
		if (dup2(data->fd[2 * data->ind], STDIN_FILENO) == -1)
			ft_perror("dup2 n1 failed in pipe_proc");
	}
	if (temp->out_next != 0)
	{
		if (ft_built_red(i, env, data, temp))
			exit (0);
		if (dup2(data->fd[2 * (data->ind + 1) + 1], STDOUT_FILENO) == -1)
			ft_perror("dup2 n2 failed in pipe_proc");
	}
	else
		if (ft_built(i, env, data, temp))
			exit (0);
}

void	red2_proc_2(t_data *data, t_process *temp, char **env, int i)
{
	if (temp->in_prev != 0 || data->inout->red_prev == 1)
	{
		if (dup2(data->fd[2 * data->ind], STDIN_FILENO) == -1)
			ft_perror("dup2 n1 failed in pipe_proc");
	}
	if (data->inout->next == NULL && temp->out_next)
	{
		if (ft_built_red(i, env, data, temp))
			exit (0);
		if (dup2(data->fd[2 * (data->ind + 1) + 1], STDOUT_FILENO) == -1)
			ft_perror("dup2 n2 failed in red_proc");
	}
	else if (data->inout->next == NULL)
	{
		if (ft_built_red2(i, env, data, temp))
			exit (0);
		if (dup2(data->file[i], STDOUT_FILENO) == -1)
			ft_perror("dup2 n3 failed in red_proc");
	}
	else
		if (ft_built(i, env, data, temp))
			exit (0);
}

void	red_proc_2(t_data *data, t_process *temp, char **env, int i)
{
	if (data->inout->next == NULL)
	{
		if (ft_built_red2(i, env, data, temp))
			exit(0);
		if (dup2(data->file[i], STDIN_FILENO) == -1)
			ft_perror("dup2 n1 failed in red2_proc");
	}
	if (data->inout->next == NULL && temp->out_next)
	{
		if (ft_built_red(i, env, data, temp))
			exit(0);
		if (dup2(data->fd[2 * (data->ind + 1) + 1], STDOUT_FILENO) == -1)
			ft_perror("dup2 n1 failed in red_proc");
	}
	else
		if (ft_built(i, env, data, temp))
			exit(0);
}

void	red3_proc_2(t_data *data, t_process *temp, char **env, int i)
{
	if (temp->out_next || (temp->inout->next && temp->inout->next->type != 3))
	{
		if (ft_built_red(i, env, data, temp))
			exit(0);
		if (dup2(data->fd[2 * (data->ind + 1) + 1], STDOUT_FILENO) == -1)
			ft_perror("dup2 n1 failed in red_proc");
	}
	else if (!ft_is_command(temp->command, "exit"))
	{
		write(2, "exit\n", 5);
		exit(0);
	}
	else
		if (ft_built(i, env, data, temp))
			exit(0);
}

void	red4_proc_2(t_data *data, t_process *temp, char **env, int i)
{
	if (data->inout->next == NULL)
	{
		if (ft_built_red2(i, env, data, temp))
			exit(0);
		if (dup2(data->file[i], STDOUT_FILENO) == -1)
			ft_perror("dup2 n1 failed in red_proc");
	}
	else
		if (ft_built_red(i, env, data, temp))
			exit(0);
}

void	pipe_proc(t_data *data, t_process *temp, char **env, int i)
{
	data->pid1[i] = fork();
	if (data->pid1[i] < 0)
		ft_perror("forking failed\n");
	if (data->pid1[i] == 0)
	{
		data->tab_args[i] = ft_dquote(ft_splitd(temp->cmd_arg, ' '), 0, 0);
		pipe_proc_2(temp, data, i, env);
		close_pipes(data);
		data->tab_paths[i] = ft_check_arg(temp->command, env);
		if (execve(data->tab_paths[i], data->tab_args[i], env) != 0)
			ft_perror("failed to exec in pipe_proc");
	}
}

void	one_proc(t_data *data, t_process *temp, char **env)
{
	data->pid1[0] = fork();
	if (data->pid1[0] < 0)
		ft_perror("forking failed\n");
	if (data->pid1[0] == 0)
	{
		data->tab_paths[0] = ft_check_arg(temp->command, env);
		close_pipes(data);
		if (execve(data->tab_paths[0], data->tab_args[0], env) == -1)
			ft_perror("failed to exec in one_proc");
	}
}

void	red2_proc(t_data *data, t_process *temp, char **env, int i)
{
	data->pid1[i] = fork();
	if (data->pid1[i] < 0)
		ft_perror("forking failed\n");
	if (data->pid1[i] == 0)
	{
		data->tab_args[i] = ft_dquote(ft_splitd(temp->cmd_arg, ' '), 0, 0);
		if (temp->out_next)
			data->fd[2 * (data->ind + 1) + 1] = open(ft_ddquote
					(data->inout->file, 0), O_RDWR | O_TRUNC | O_CREAT, 0644);
		else
			data->file[i] = open(ft_ddquote
					(data->inout->file, 0), O_RDWR | O_TRUNC | O_CREAT, 0644);
		if (data->fd[2 * (data->ind + 1) + 1] < 0)
			ft_perror("\033[2K\r\033[0;31mError\033[0m : file creation failed");
		red2_proc_2(data, temp, env, i);
		close_pipes(data);
		data->tab_paths[i] = ft_check_arg(temp->command, env);
		if (temp->command != NULL)
		{
			if (data->inout->next == NULL)
				if (execve(data->tab_paths[i], data->tab_args[i], env) == -1)
					ft_perror("failed to exec in red_proc\n");
		}
		exit(0);
	}
}

void	red_proc(t_data *data, t_process *temp, char **env, int i)
{
	data->pid1[i] = fork();
	if (data->pid1[i] < 0)
		ft_perror("forking failed\n");
	if (data->pid1[i] == 0)
	{
		data->file[i] = open(ft_ddquote(data->inout->file, 0), O_RDONLY);
		if (data->file[i] < 0)
			ft_perror("\033[2K\r\033[0;31mError\033[0m : file opening failed");
		data->tab_args[i] = ft_dquote(ft_splitd(temp->cmd_arg, ' '), 0, 0);
		red_proc_2(data, temp, env, i);
		close_pipes(data);
		close(data->file[i]);
		data->tab_paths[i] = ft_check_arg(temp->command, env);
		if (temp->command != NULL)
		{
			if (data->inout->next == NULL)
				if (execve(data->tab_paths[i], data->tab_args[i], env) == -1)
					ft_perror("failed to exec in red2_proc\n");
		}
		exit (0);
	}
}

void	red3_proc(t_data *data, t_process *temp, char **env, int i)
{
	data->pid1[i] = fork();
	if (data->pid1[i] < 0)
		ft_perror("forking failed\n");
	if (data->pid1[i] == 0)
	{
		data->tab_args[i] = ft_dquote(ft_splitd(temp->cmd_arg, ' '), 0, 0);
		if (dup2(data->fd[2 * (data->ind + 1) - 2], STDIN_FILENO) == -1)
			ft_perror("dup2 n1 failed in red3_proc");
		red3_proc_2(data, temp, env, i);
		close_pipes(data);
		data->tab_paths[i] = ft_check_arg(temp->command, env);
		if (temp->command != NULL && !data->inout->next)
			if (execve(data->tab_paths[i], data->tab_args[i], env) == -1)
				ft_perror("failed to exec in red3_proc\n");
		exit(0);
	}
}

void	red4_proc(t_data *data, t_process *temp, char **env, int i)
{
	data->pid1[i] = fork();
	if (data->pid1[i] < 0)
		ft_perror("forking failed\n");
	if (data->pid1[i] == 0)
	{
		data->file[i] = open(ft_ddquote(data->inout->file, 0),
				O_RDWR | O_APPEND | O_CREAT, 0644);
		if (data->file[i] < 0)
			ft_perror("\033[2K\r\033[0;31mError\033[0m : file creation failed");
		data->tab_args[i] = ft_dquote(ft_splitd(temp->cmd_arg, ' '), 0, 0);
		if (temp->in_prev != 0)
			if (dup2(data->fd[2 * data->ind - 2], STDIN_FILENO) == -1)
				ft_perror("dup2 n1 failed in pipe_proc");
		red4_proc_2(data, temp, env, i);
		close_pipes(data);
		close(data->file[i]);
		data->tab_paths[i] = ft_check_arg(temp->command, env);
		if (temp->command != NULL)
		{
			if (data->inout->next == NULL)
				if (execve(data->tab_paths[i], data->tab_args[i], env) == -1)
					ft_perror("failed to exec in red_proc\n");
		}
		exit(0);
	}
}

int	ft_execute_cmd_3(t_process *temp, t_process *proc, t_inout *temp2, int i)
{
	while (temp->next != NULL)
	{
		temp = temp->next;
		temp2 = temp->inout;
		if (temp2)
		{
			while (temp2)
			{
				if (temp2 && temp2->type != 3)
				{
					i++;
					temp2 = temp2->next;
				}
				if (temp2 && temp2->type == 3)
				{
					i++;
					while (temp2 && temp2->type == 3)
						temp2 = temp2->next;
				}
			}
		}
		else if (temp)
			i++;
	}
	return (i);
}

int	ft_execute_cmd_2(t_process *temp, t_process *proc, t_inout *temp2)
{
	int	i;

	if (!temp2 || (temp2 && temp2->type == 3))
		i = 1;
	else
		i = 0;
	while (temp2)
	{
		if (temp2 && temp2->type != 3)
			i++;
		temp2 = temp2->next;
	}
	i = ft_execute_cmd_3(temp, proc, temp2, i);
	temp = proc;
	return (i);
}

int	ft_execute_cond(t_process *temp, t_data *data, int i, char **env)
{
	data->tab_args[0] = ft_dquote(ft_splitd(temp->cmd_arg, ' '), 0, 0);
	if (!ft_is_command(temp->command, "exit"))
	{
		write(2, "exit\n", 5);
		free_exec(data, i);
		exit(0);
	}
	if (!ft_built(0, env, data, temp))
		one_proc(data, temp, env);
	else
	{
		free_exec(data, i);
		return (0);
	}
	data->j = 0;
	return (1);
}

int	ft_execute_cmd_7(t_data *data, t_process *temp, char **env, int i)
{
	while (i >= 0 && data->inout != 0 && data->inout->type == 1)
	{
		red_proc(data, temp, env, i);
		data->inout = data->inout->next;
		if (!data->inout && !temp->out_next
			&& !ft_is_command(temp->command, "exit"))
			exit(0);
		i--;
	}
	return (i);
}

int	ft_execute_cmd_6(t_data *data, t_process *temp, char **env, int i)
{
	while (i >= 0 && (!data->inout))
	{
		pipe_proc(data, temp, env, i);
		temp = temp->next;
		data->ind++;
		i--;
		if (temp)
			data->inout = temp->inout;
	}
	while (i >= 0 && data->inout != 0 && data->inout->type == 2)
	{
		red2_proc(data, temp, env, i);
		data->inout = data->inout->next;
		if (!data->inout && !temp->out_next
			&& !ft_is_command(temp->command, "exit"))
			exit(0);
		i--;
	}
	i = ft_execute_cmd_7(data, temp, env, i);
	return (i);
}

void	ft_execute_cmd_8(t_data *data, t_process *temp, t_inout *tmp)
{
	data->line = NULL;
	close(data->fd[2 * data->ind + 1]);
	if (pipe(data->fd + 2 * data->ind) < 0)
		ft_perror("nooope");
	while (data->inout->next != NULL && data->inout->next->type == 3)
		data->inout = data->inout->next;
	if (!ft_is_command(tmp->file, data->inout->file))
	{
		data->line = readline("heredoc> ");
		if (ft_is_command(data->line, data->inout->file))
		{
			write(data->fd[2 * data->ind + 1], data->line
				, ft_strlen(data->line));
			write(data->fd[2 * data->ind + 1], "\n", 1);
		}
	}
	while (ft_is_command(tmp->file, data->inout->file))
	{
		data->line = readline("heredoc> ");
		if (!ft_is_command(data->line, tmp->file)
			&& tmp->next && tmp->next->type == 3)
			tmp = tmp->next;
		if (ft_is_command(tmp->file, data->inout->file))
			free(data->line);
	}
}

void	ft_execute_cmd_9(t_data *data, t_process *temp, char **env, int i)
{
	while (ft_is_command(data->line, data->inout->file))
	{
		if (data->line)
			free (data->line);
		data->line = readline("heredoc> ");
		if (ft_is_command(data->line, data->inout->file))
		{
			write(data->fd[2 * data->ind + 1], data->line
				, ft_strlen(data->line));
			write(data->fd[2 * data->ind + 1], "\n", 1);
		}
	}
	free(data->line);
	red3_proc(data, temp, env, i);
	data->inout = data->inout->next;
	if (!data->inout && !temp->out_next
		&& !ft_is_command(temp->command, "exit"))
		exit(0);
}

void	ft_execute_cmd_10(t_data *data, t_process *temp, char **env, int i)
{
	if (i >= 0 && data->inout != 0 && data->inout->type == 4)
	{
		red4_proc(data, temp, env, i);
		data->inout = data->inout->next;
		if (!data->inout && !temp->out_next
			&& !ft_is_command(temp->command, "exit"))
			exit(0);
		i--;
	}
	if (temp && temp->inout && !data->inout)
	{
		temp = temp->next;
		data->ind++;
	}
}

void	ft_execute_cmd_4(t_data *data, t_process *temp, char **env, int i)
{
	t_inout		*tmp;

	while (i >= 0)
	{
		if (!data->inout)
			data->inout = temp->inout;
		i = ft_execute_cmd_6(data, temp, env, i);
		if (i >= 0 && data->inout != 0 && data->inout->type == 3)
		{
			tmp = data->inout;
			ft_execute_cmd_8(data, temp, tmp);
			ft_execute_cmd_9(data, temp, env, i);
			i--;
		}
		ft_execute_cmd_10(data, temp, env, i);
	}
}

int	ft_execute_cmd_5(t_data *data, t_process *temp, char **env, int i)
{
	if (i == 1 && temp->inout == 0)
	{
		if (ft_execute_cond(temp, data, i, env) != 1)
			return (0);
	}
	else
	{
		i--;
		data->j = i;
		data->ind = 0;
		data->inout = NULL;
		ft_execute_cmd_4(data, temp, env, i);
	}
	return (1);
}

int	ft_execute_cmd(t_process *proc, char **env, char **penv)
{
	t_data		data;
	t_process	*temp;
	t_inout		*temp2;
	int			i;

	data.tab_env = penv;
	temp = proc;
	temp2 = temp->inout;
	i = ft_execute_cmd_2(temp, proc, temp2);
	if (!ft_malloc_struct(&data, i))
		ft_perror("malloc failed\n");
	create_pipes(&data);
	if (ft_execute_cmd_5(&data, temp, env, i) == 0)
		return (0);
	i = data.j;
	close_pipes(&data);
	while (data.j >= 0)
	{
		waitpid(data.pid1[data.j], &data.status, 0);
		data.j--;
	}
	free_exec(&data, i);
	return (0);
}
