/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipex.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asaffroy <asaffroy@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/01/03 11:15:22 by asaffroy          #+#    #+#             */
/*   Updated: 2022/01/03 11:36:20 by asaffroy         ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/pipex.h"

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
	return (i);
}

char	*ft_check_arg(char *cmd, char **env)
{
	int		i;
	char	**tab;
	char	*try;

	if (!access(cmd, F_OK))
	{
		if (!access(cmd, R_OK))
			return (cmd);
		else
			ft_perror("\033[2K\r\033[0;31mError\033[0m : permission denied");
	}
	cmd = ft_strjoin("/", cmd);
	i = find_path(env);
	tab = ft_split(env[i] + 5, ':');
	i = 0;
	while (tab[i])
	{
		try = ft_strjoin(tab[i], cmd);
		if (!access(try, X_OK))
			return (try);
		i++;
	}
	ft_perror("command not found");
	return (0);
}

void	father_proc(t_data *data, char **argv, char **env)
{
	close (data->fd[1]);
	wait(NULL);
	data->file2 = open(argv[4], O_RDWR | O_CREAT, 0644);
	if (data->file2 < 0)
		ft_perror("\033[2K\r\033[0;31mError\033[0m : outfile creation failed");
	data->args_of_2 = ft_split(argv[3], ' ');
	data->path_of_2 = ft_check_arg(data->args_of_2[0], env);
	if (dup2(data->file2, STDOUT_FILENO) == -1)
		ft_perror("dup2 n1 failed in father_proc");
	if (dup2(data->fd[0], STDIN_FILENO) == -1)
		ft_perror("dup2 n2 failed in father_proc");
	close(data->fd[0]);
	close(data->file2);
	if (execve(data->path_of_2, data->args_of_2, env) == -1)
		ft_perror("failed to exec in father_proc");
}

void	child_proc(t_data *data, char **argv, char **env)
{
	close (data->fd[0]);
	data->file1 = open(argv[1], O_RDONLY);
	if (data->file1 < 0)
		ft_perror("\033[2K\r\033[0;31mError\033[0m : couldn't find infile");
	data->args_of_1 = ft_split(argv[2], ' ');
	data->path_of_1 = ft_check_arg(data->args_of_1[0], env);
	if (dup2(data->file1, STDIN_FILENO) == -1)
		ft_perror("dup2 n1 failed in child_proc");
	if (dup2(data->fd[1], STDOUT_FILENO) == -1)
		ft_perror("dup2 n2 failed in child_proc");
	close(data->fd[1]);
	close(data->file1);
	if (execve(data->path_of_1, data->args_of_1, env) == -1)
		ft_perror("failed to exec in child_proc");
}

int	ft_execute_cmd(int argc, char **argv, char **env, t_talistechainee *lambda)
{
	int			pid1;
	t_data		data;

	if (argc != 5)
		ft_perror
		("\033[2K\r\033[0;35musage\033[0m : ./pipex file1 \"cmd1\" \"cmd2\" file2");
	else
	{
		if (pipe(data.fd) != 0)
			ft_perror("pipe failed\n");
		pid1 = fork();
		if (pid1 < 0)
			ft_perror("forking failed\n");
		if (pid1 == 0)
			child_proc(&data, argv, env);
		else
			father_proc(&data, argv, env);
	}
	return (0);
}
