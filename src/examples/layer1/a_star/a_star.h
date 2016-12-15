namespace octet {

	class a_star_app;
	class a_star
	{

	public:
		class grid
		{
			int x;
			int y;
			float g;
			float h;
			float v;
			float f;
			grid *came_from;

		public:
			grid() :
				g(0),
				h(0),
				f(0),
				v(0),
				came_from(NULL)
			{
			}

			grid(int x, int y) :
				x(x),
				y(y)
			{
			}

			void set_came_from(grid *c)
			{
				came_from = c;
			}

			grid *get_came_from()
			{
				return came_from;
			}

			void set_position(int x, int y)
			{
				this->x = x;
				this->y = y;
			}

			int get_x()
			{
				return x;
			}

			int get_y()
			{
				return y;
			}

			void compute_h(int target_x, int target_y)
			{
				int dx = target_x - x, dy = target_y - y;
				h = sqrt((float)dx * dx + dy * dy) + v;
			}

      float get_h()
      {
        return h;
      }

      void set_v(float v)
      {
        this->v = v;
      }

			void set_g(float g)
			{
				this->g = g;
				f = g + h;
			}

			float get_f()
			{
				return f;
			}

			float get_g()
			{
				return g;
			}
		};

	private:
		int size;
		grid *grids;
		grid *origin;
		grid *target;
		grid *current;
		dynarray<grid*> close_set;
		dynarray<grid*> open_set;
		dynarray<grid*> came_from;
		dynarray<grid*> path;

	public:
		a_star(int size);

		~a_star()
		{
			if(grids != NULL)
				delete grids;
		}
		dynarray<grid*> &get_path()
		{
			return path;
		}

    unsigned int get_grid_count()
    {
      return path.size();
    }
		
		grid* get_grid(unsigned int index)
		{
			if(index >= path.size())
				return NULL;
			return path[index];
		}

    void clear()
    {
      came_from.reset();
      close_set.reset();
      open_set.reset();
      path.reset();
      int index = 0;
      for(int i = 0; i < size; i++)
      {
        for(int j = 0; j < size; j++)
        {
          grids[index].set_v(0);
          grids[index].set_came_from(NULL);
          grids[index++].set_position(j, i);
        }
      }
    }

		void set_block(int x, int y);

		void generate_path();

		bool is_in_open_set(grid *n)
		{
			unsigned int i = 0;
			for(; i < open_set.size(); i++)
			{
				if(open_set[i] == n)
					break;
			}
			if(i == open_set.size())
				return false;
			return true;
		}

		bool is_in_close_set(grid *n)
		{
			unsigned int i = 0;
			for(; i < close_set.size(); i++)
			{
				if(close_set[i] == n)
					break;
			}
			if(i == close_set.size())
				return false;
			return true;
		}

		void update_neighbour(int x, int y, float v = 1)
		{
			grid *n = grids + x + y * size;
			if(is_in_close_set(n))
			{
				return;
			}
			float tentative_g = v + current->get_g();
			bool b;
			if(!(b = is_in_open_set(n)) || n->get_g() > tentative_g)
			{
				n->set_g(tentative_g);
				n->set_came_from(current);
				if(!b)
				{
					open_set.push_back(n);
				}
			}
		}

		void search()
		{
			if(origin == NULL || target == NULL)
			{
				return;
			}
			for(int i = 0; i < size * size; i++)
			{
				grids[i].compute_h(target->get_x(), target->get_y());
			}
			open_set.push_back(origin);
			while(!open_set.is_empty())
			{
				int index = 0;
				current = open_set[0];
				for(unsigned int i = 1; i < open_set.size(); i++)
				{
					if(current->get_f() > open_set[i]->get_f())
					{
						current = open_set[i];
						index = i;
					}
				}
				if(current == target)
					break;
				close_set.push_back(current);
				open_set.erase(index);
        if(current->get_h() >= 999999)
        {
          continue;
        }
				if(current->get_y() < size - 1)
					update_neighbour(current->get_x(), current->get_y() + 1);
				if(current->get_x() < size - 1)
					update_neighbour(current->get_x() + 1, current->get_y());
				if(current->get_y() > 0)
					update_neighbour(current->get_x(), current->get_y() - 1);
				if(current->get_x() > 0)
					update_neighbour(current->get_x() - 1, current->get_y());
				if(current->get_x() > 0 && current->get_y() > 0)
					update_neighbour(current->get_x() - 1, current->get_y() - 1, 1.414f);
				if(current->get_x() > 0 && current->get_y() < size - 1)
					update_neighbour(current->get_x() - 1, current->get_y() + 1, 1.414f);
				if(current->get_x() < size - 1 && current->get_y() < size - 1)
					update_neighbour(current->get_x() + 1, current->get_y() + 1, 1.414f);
				if(current->get_x() < size - 1 && current->get_y() > 0)
					update_neighbour(current->get_x() + 1, current->get_y() - 1, 1.414f);
			}
			generate_path();
		}

		void set_origin(int x, int y)
		{
      if(x < 0 || x > size - 1 || y < 0 || y > size - 1)
        return;
			origin = grids + y * size + x;
		}

		void set_target(int x, int y)
		{
      if(x < 0 || x > size - 1 || y < 0 || y > size - 1)
        return;
			target = grids + y * size + x;
		}
	};
}
