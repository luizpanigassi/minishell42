/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: luinasci <luinasci@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/17 09:53:29 by jcologne          #+#    #+#             */
/*   Updated: 2025/05/08 16:30:40 by luinasci         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <minishell.h>

/**
 * @brief Initializes the parser state for new input.
 * @param p Parser structure to initialize.
 * @param input Input string to parse.
 */
void	init_parser(t_parse *p, char *input)
{
	p->input = input;
	p->pos = 0;
	p->curr_char = input[0];
	p->token_type = T_EOF;
	p->token_value = NULL;
	p->redir_fd = 0;
	p->syntax_error = 0;
}

/**
 * @brief Initializes the word and quote states for handle_word.
 * @param p Parser state.
 * @param word_state Pointer to the word state structure.
 * @param quote_state Pointer to the quote state structure.
 */
void	init_word_and_quote_states(t_parse *p,
	t_word_state *word_state, t_quote_state *quote_state)
{
word_state->segment_start = p->pos;
word_state->in_quote = 0;
word_state->quote_char = 0;
word_state->value = ft_strdup("");
quote_state->segment_start = &word_state->segment_start;
quote_state->in_quote = &word_state->in_quote;
quote_state->quote_char = &word_state->quote_char;
}

/**
 * @brief Initializes pipeline resources such as pipes and child process array.
 * @param pipeline Linked list of commands to execute.
 * @param prev_pipe Array representing the previous pipe.
 * @param next_pipe Array representing the next pipe.
 * @param child_pids Pointer to the array of child process IDs.
 * @return 0 on success, 1 on failure.
 */
int	initialize_pipeline_resources(t_cmd *pipeline, int prev_pipe[2],
	int next_pipe[2], pid_t **child_pids)
{
	int	cmd_count;

	prev_pipe[0] = -1;
	prev_pipe[1] = -1;
	next_pipe[0] = -1;
	next_pipe[1] = -1;
	cmd_count = ft_cmd_size(pipeline);
	if (cmd_count == 0)
		return (1);
	*child_pids = malloc(sizeof(pid_t) * cmd_count);
	if (!*child_pids)
		return (1);
	return (0);
}

/**
 * @brief Initializes the pipeline context and resources.
 * @param pipeline Linked list of commands to execute.
 * @param ctx Pointer to the pipeline context to initialize.
 * @param old_sa Pointer to the old sigaction structure to restore later.
 * @return 0 on success, 1 on failure.
 */
int	initialize_pipeline(t_cmd *pipeline,
	t_pipeline_context *ctx, struct sigaction *old_sa)
{
	if (initialize_pipeline_resources(pipeline, ctx->prev_pipe,
			ctx->next_pipe, &ctx->child_pids))
	{
		sigaction(SIGINT, old_sa, NULL);
		return (1);
	}
	ctx->index = 0;
	ctx->current = pipeline;
	return (0);
}
