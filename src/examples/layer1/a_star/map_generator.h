namespace octet {

	class map_generator
	{
    struct room
    {
      int x;
      int y;
      int width;
      int height;
      room()
      {
      }

      room(int x, int y, int width, int height) :
        x(x),
        y(y),
        width(width),
        height(height)
      {

      }
    };

    dynarray<room> rooms;
    dynarray<int> grids;
    //grid count in each row;
    int grid_count;
	public:
		map_generator(int size) :
      grid_count(size)
		{
      grids.resize(grid_count * grid_count);
			srand((unsigned int)time(NULL));
		}

    void clear()
    {
      for(unsigned int i = 0; i < grids.size(); i++)
      {
        grids[i] = 0;
      }
      rooms.reset();
    }

    int get_grid(int x, int y)
    {
      return grids[y * grid_count + x];
    }

    void set_grid(int x, int y, int v = 1)
    {
      grids[y * grid_count + x] = v;
    }

    void connect_room(int index1, int index2)
    {
      static const int half_hall_width = 1;
      const float d = 0.05f;
      int w = rand() % (rooms[index1].width - 2 * half_hall_width);
      int w1 = rand() % (rooms[index2].width - 2 * half_hall_width);
      int h = rand() % (rooms[index1].height - 2 * half_hall_width);
      int h1 = rand() % (rooms[index2].height - 2 * half_hall_width);
      int x1 = rooms[index1].x + half_hall_width + w, y1 = rooms[index1].y + half_hall_width + h, x2 = rooms[index2].x + half_hall_width + w1, y2 = rooms[index2].y + half_hall_width + h1;
      int dy = y1 < y2 ? 1 : -1;
      int dx = x1 < x2 ? 1 : -1;
      for(int i = x1 - half_hall_width; i < x1; i++)
      {
        for(int j = y1; j != y2 + dy; j += dy)
        {
          set_grid(i, j);
        }
      }
      for(int i = x1 - half_hall_width; i != x2 + dx; i += dx)
      {
        for(int j = y2 - half_hall_width; j < y2; j++)
        {
          set_grid(i, j);
        }
      }
    }

    void reset()
    {

    }

    unsigned int bsp(int pos_x, int pos_y, int width, int height, int iteration)
    {
      const int MIN = 4, DMIN = MIN * 2;
			if(iteration-- == 0 || width <= DMIN || height <= DMIN)
			{
        int width1 = MIN + rand() % (width - MIN + 1);
        int height1 = MIN + rand() % (height - MIN + 1);
				int pos_x1 = pos_x + rand() % (width - width1 + 1), pos_y1 = pos_y + rand() %(height - height1 + 1);
        for(int i = pos_x1; i < pos_x1 + width1; i++)
        {
          for(int j = pos_y1; j < pos_y1 + height1; j++)
          {
            set_grid(i, j);
          }
        }
        rooms.push_back(room(pos_x1, pos_y1, width1, height1));
        return rooms.size() - 1;
			}
			int res0, res1;
      int w, h, w1, h1, pos_x1, pos_y1;
			if(width > height)
			{
        w = rand() % (width - DMIN + 1) + MIN;
        w1 = width - w;
        h1 = h = height;
        pos_x1 = pos_x + w;
        pos_y1 = pos_y;
			}
			else
			{
        w1 = w = width;
        h = rand() % (height - DMIN + 1) + MIN;
        h1 = height - h;
        pos_x1 = pos_x;
        pos_y1 = pos_y + h;
			}
      res0 = bsp(pos_x, pos_y, w, h, iteration);
      res1 = bsp(pos_x1, pos_y1, w1, h1, iteration);
      connect_room(res0, res1);
			return rand() % 2 ? res0 : res1;
    }


	};

}

